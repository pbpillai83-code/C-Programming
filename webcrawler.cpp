/****************************************************
 * P1.cpp — Relational Database Engine
 ****************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

enum class DataType {
    STRING,
    DOUBLE
};

class Row {
public:
    std::vector<std::string> values;
    Row(const std::vector<std::string>& v) : values(v) {}
};

class Table {
private:
    std::string name;
    std::vector<std::string> columns;
    std::vector<DataType> types;
    std::string primaryKey;
    std::vector<Row> rows;

public:
    Table(const std::string& n,
          const std::vector<std::string>& cols,
          const std::vector<DataType>& t,
          const std::string& pk)
        : name(n), columns(cols), types(t), primaryKey(pk) {}

    const std::string& getName() const { return name; }
    const std::vector<std::string>& getColumns() const { return columns; }
    const std::vector<Row>& getRows() const { return rows; }

    void insertRow(const std::vector<std::string>& vals,
                   const std::unordered_map<std::string, Table*>& allTables)
    {
        if (vals.size() != columns.size()) {
            std::cout << "Insert failed: column count mismatch.\n";
            return;
        }
        rows.emplace_back(vals);
    }

    void display() const {
        std::cout << "\n=== TABLE: " << name << " ===\n";
        for (const auto& col : columns)
            std::cout << col << " | ";
        std::cout << "\n---------------------------------------------\n";

        for (const auto& r : rows) {
            for (const auto& v : r.values)
                std::cout << v << " | ";
            std::cout << "\n";
        }
        std::cout << "---------------------------------------------\n";
    }
};

class Database {
private:
    std::unordered_map<std::string, Table*> tables;

public:
    ~Database() {
        for (auto& p : tables)
            delete p.second;
    }

    void createTableWithColumns(const std::string& name,
                                const std::vector<std::pair<std::string, DataType>>& cols,
                                const std::string& primaryKey)
    {
        std::vector<std::string> colNames;
        std::vector<DataType> colTypes;

        for (const auto& c : cols) {
            colNames.push_back(c.first);
            colTypes.push_back(c.second);
        }

        tables[name] = new Table(name, colNames, colTypes, primaryKey);
        std::cout << "Created table: " << name << "\n";
    }

    Table* getTable(const std::string& name) {
        if (tables.count(name))
            return tables[name];
        return nullptr;
    }

    std::unordered_map<std::string, Table*> getTablePointers() const {
        return tables;
    }

    void listTables() const {
        std::cout << "\n=== DATABASE TABLES ===\n";
        for (const auto& p : tables)
            std::cout << p.first << "\n";
        std::cout << "========================\n";
    }
};
/************************************************************
 * P2.cpp — Web Crawler + NVD Batch + PDF Export + ALL Vendors
 ************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <hpdf.h>

#include "P1.cpp"

using json = nlohmann::json;

/**************** CURL Write Callback ****************/
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t total = size * nmemb;
    s->append((char*)contents, total);
    return total;
}

/**************** Fetch URL ****************/
std::string fetchURL(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

/**************** CVE Struct ****************/
struct CVEDetail {
    std::string cve_id;
    std::string description;
    double cvss_score = 0.0;
    std::string severity = "Unknown";
    std::string cwe_id = "Unknown";
};

/**************** Trim Description ****************/
std::string trimDesc(const std::string& d) {
    if (d.size() <= 15) return d;
    return d.substr(0, 15);
}

/**************** Format CVSS Score (one decimal) ****************/
std::string formatCVSS(double score) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << score;
    return ss.str();
}

/**************** Extract CVEs ****************/
std::vector<std::string> extractCVEsFromHTML(const std::string& html) {
    std::vector<std::string> ids;
    std::regex r("CVE-\\d{4}-\\d+");

    auto b = std::sregex_iterator(html.begin(), html.end(), r);
    auto e = std::sregex_iterator();

    for (auto it = b; it != e; ++it)
        ids.push_back(it->str());

    return ids;
}

/**************** NVD Batch Fetch ****************/
std::unordered_map<std::string, CVEDetail> fetchNVD_Batch(const std::vector<std::string>& cveList) {
    std::unordered_map<std::string, CVEDetail> result;
    const int batchSize = 20;

    for (size_t i = 0; i < cveList.size(); i += batchSize) {
        std::string url = "https://services.nvd.nist.gov/rest/json/cves/2.0?";
        size_t end = std::min(i + batchSize, cveList.size());

        for (size_t j = i; j < end; j++)
            url += "cveId=" + cveList[j] + "&";

        std::string resp = fetchURL(url);
        if (resp.empty()) continue;

        try {
            auto j = json::parse(resp);
            if (!j.contains("vulnerabilities")) continue;

            for (auto& v : j["vulnerabilities"]) {
                auto cve = v["cve"];
                CVEDetail d;

                d.cve_id = cve["id"].get<std::string>();

                if (cve.contains("descriptions") && !cve["descriptions"].empty())
                    d.description = trimDesc(cve["descriptions"][0]["value"].get<std::string>());

                if (cve.contains("metrics") &&
                    cve["metrics"].contains("cvssMetricV31") &&
                    !cve["metrics"]["cvssMetricV31"].empty()) {

                    auto m = cve["metrics"]["cvssMetricV31"][0];
                    d.cvss_score = m["cvssData"]["baseScore"].get<double>();
                    d.severity   = m["cvssData"]["baseSeverity"].get<std::string>();
                }

                if (cve.contains("weaknesses") && !cve["weaknesses"].empty()) {
                    auto w = cve["weaknesses"][0];
                    if (w.contains("description") && !w["description"].empty())
                        d.cwe_id = w["description"][0]["value"].get<std::string>();
                }

                result[d.cve_id] = d;
            }

        } catch (...) {}
    }

    return result;
}

/**************** Vendor URL Sets ****************/
std::vector<std::string> mongoURLs(int year) {
    return {
        "http://mongodb.com/resources/products/alerts?msockid=0208f0da1ce9601a1558e6731d8b61c7#security"
    };
}

std::vector<std::string> mysqlURLs(int year) {
    if (year == 2026) {
        return {
            "https://www.oracle.com/security-alerts/cpuJan2026.html#AppendixMSQL",
            "https://www.oracle.com/security-alerts/cpuapr2026.html#AppendixMSQL",
            "https://www.oracle.com/security-alerts/cpujul2026.html#AppendixMSQL",
            "https://www.oracle.com/security-alerts/cpuoct2026.html#AppendixMSQL"
        };
    }
    return {};
}

std::vector<std::string> mariadbURLs(int year) {
    return {
        "https://mariadb.com/docs/server/security/cve/enterprise-server"
    };
}

std::vector<std::string> postgresURLs(int year) {
    return {
        "https://www.postgresql.org/support/security/"
    };
}

std::vector<std::string> oracleDBURLs(int year) {
    if (year == 2026) {
        return {
            "https://www.oracle.com/security-alerts/cpujan2026.html#AppendixDB",
            "https://www.oracle.com/security-alerts/cpuapr2026.html#AppendixDB",
            "https://www.oracle.com/security-alerts/cpujul2026.html#AppendixDB",
            "https://www.oracle.com/security-alerts/cpuoct2026.html#AppendixDB"
        };
    }
    return {};
}

std::vector<std::string> mssqlURLs(int year) {
    return {
        "https://sqlserverbuilds.blogspot.com/",
        "https://sqlserverbuilds.blogspot.com/#sql2022x",
        "https://sqlserverbuilds.blogspot.com/#sql2025x",
        "https://sqlserverbuilds.blogspot.com/#sql2019x",
        "https://sqlserverbuilds.blogspot.com/#sql2017x"
    };
}

std::vector<std::string> sapHANAURLs(int year) {
    if (year == 2026) {
        return {
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/january-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/february-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/march-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/april-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/may-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/june-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/july-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/august-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/september-2026.html?isu_page=1",
            "https://support.sap.com/en/my-support/knowledge-base/security-notes-news/october-2026.html?isu_page=1"
        };
    }
    return {};
}

std::vector<std::string> ibmDB2URLs(int year) {
    return {
        "https://www.ibm.com/support/pages/published-security-vulnerabilities-db2-linux-unix-and-windows-including-security-update-information"
    };
}

/**************** Vendor Router ****************/
std::vector<std::string> getVendorURLs(const std::string& vendor, int year) {

    if (vendor == "MongoDB")        return mongoURLs(year);
    if (vendor == "MySQL")          return mysqlURLs(year);
    if (vendor == "MariaDB")        return mariadbURLs(year);
    if (vendor == "PostgreSQL")     return postgresURLs(year);
    if (vendor == "Oracle Database")return oracleDBURLs(year);
    if (vendor == "MSSQL")          return mssqlURLs(year);
    if (vendor == "SAP HANA")       return sapHANAURLs(year);
    if (vendor == "IBM DB2")        return ibmDB2URLs(year);

    return {};
}

/**************** Fetch Vendor CVEs ****************/
std::vector<CVEDetail> fetchVendorCVEs(const std::string& vendor, int year) {
    std::vector<std::string> allIDs;

    auto urls = getVendorURLs(vendor, year);

    for (const auto& url : urls) {
        std::cout << "Fetching " << vendor << " from: " << url << "\n";
        std::string html = fetchURL(url);
        if (html.empty()) continue;

        auto ids = extractCVEsFromHTML(html);
        allIDs.insert(allIDs.end(), ids.begin(), ids.end());
    }

    auto enrichedMap = fetchNVD_Batch(allIDs);

    std::vector<CVEDetail> finalList;
    for (const auto& id : allIDs) {
        if (enrichedMap.count(id))
            finalList.push_back(enrichedMap[id]);
    }

    return finalList;
}

/**************** PDF Error Handler ****************/
void pdfErrorHandler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    std::cout << "PDF Error: " << error_no << " detail: " << detail_no << "\n";
}

/**************** Draw Bordered Table Row ****************/
void drawTableRow(HPDF_Page page, float x, float y,
                  const std::vector<std::string>& cols,
                  const std::vector<float>& widths)
{
    float height = 18;

    HPDF_Page_SetLineWidth(page, 0.5);

    float curX = x;

    for (size_t i = 0; i < cols.size(); i++) {
        HPDF_Page_Rectangle(page, curX, y - height, widths[i], height);
        HPDF_Page_Stroke(page);

        HPDF_Page_BeginText(page);
        HPDF_Page_MoveTextPos(page, curX + 3, y - 14);
        HPDF_Page_ShowText(page, cols[i].c_str());
        HPDF_Page_EndText(page);

        curX += widths[i];
    }
}

/**************** Export Combined PDF ****************/
void exportAllVendorPDF(Database& db) {

    HPDF_Doc pdf = HPDF_New(pdfErrorHandler, nullptr);
    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

    HPDF_Font font = HPDF_GetFont(pdf, "Courier", "WinAnsiEncoding");

    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetFontAndSize(page, font, 18);

    float width = HPDF_Page_GetWidth(page);
    float height = HPDF_Page_GetHeight(page);

    /**************** COVER PAGE ****************/
    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, 50, height - 80);
    HPDF_Page_ShowText(page, "PCNC LLC – Vulnerability Intelligence Report");

    HPDF_Page_MoveTextPos(page, 0, -30);
    HPDF_Page_ShowText(page, "Database Vendor CVEs – Combined Report");

    HPDF_Page_MoveTextPos(page, 0, -30);
    HPDF_Page_ShowText(page, "Prepared for: Prashanth B Pillai");

    HPDF_Page_MoveTextPos(page, 0, -30);
    HPDF_Page_ShowText(page, "Generated on: 18 October 2026");

    HPDF_Page_EndText(page);

    /**************** NEW PAGE FOR CONTENT ****************/
    page = HPDF_AddPage(pdf);
    HPDF_Page_SetFontAndSize(page, font, 12);

    float y = height - 40;

    std::vector<std::string> vendorTables = {
        "webcrawler_MongoDB",
        "webcrawler_MySQL",
        "webcrawler_MariaDB",
        "webcrawler_SAP_HANA",
        "webcrawler_Oracle_Database",
        "webcrawler_MSSQL",
        "webcrawler_PostgreSQL",
        "webcrawler_IBM_DB2"
    };

    for (const auto& tableName : vendorTables) {

        Table* t = db.getTable(tableName);
        if (!t) continue;

        /******** Section Header ********/
        HPDF_Page_BeginText(page);
        HPDF_Page_MoveTextPos(page, 50, y);
        HPDF_Page_ShowText(page, ("=== " + tableName + " ===").c_str());
        HPDF_Page_EndText(page);

        y -= 30;

        std::vector<std::string> headers = {
            "CVE ID", "Description", "CVSS", "Severity", "CWE"
        };

        std::vector<float> widths = {120, 120, 60, 80, 120};

        drawTableRow(page, 50, y, headers, widths);
        y -= 20;

        for (const auto& r : t->getRows()) {

            if (y < 60) {
                page = HPDF_AddPage(pdf);
                HPDF_Page_SetFontAndSize(page, font, 12);
                y = height - 40;
            }

            drawTableRow(page, 50, y, r.values, widths);
            y -= 20;
        }

        y -= 40;
    }

    HPDF_SaveToFile(pdf, "CVE_Report_All_Vendors.pdf");
    HPDF_Free(pdf);

    std::cout << "\nPDF generated: CVE_Report_All_Vendors.pdf\n";
}

/**************** Generate Single Vendor Table (Menu option 3) ****************/
void generateWebCrawler(Database& db) {
    int year;
    std::cout << "Enter year (e.g., 2026): ";
    std::cin >> year;

    std::cout << "Select vendor:\n";
    std::cout << "1. MongoDB\n";
    std::cout << "2. MySQL\n";
    std::cout << "3. MariaDB\n";
    std::cout << "4. SAP HANA\n";
    std::cout << "5. Oracle Database\n";
    std::cout << "6. MSSQL\n";
    std::cout << "7. PostgreSQL\n";
    std::cout << "8. IBM DB2\n";
    std::cout << "Choice: ";

    int v;
    std::cin >> v;

    std::string vendor;
    switch (v) {
        case 1: vendor = "MongoDB"; break;
        case 2: vendor = "MySQL"; break;
        case 3: vendor = "MariaDB"; break;
        case 4: vendor = "SAP HANA"; break;
        case 5: vendor = "Oracle Database"; break;
        case 6: vendor = "MSSQL"; break;
        case 7: vendor = "PostgreSQL"; break;
        case 8: vendor = "IBM DB2"; break;
        default:
            std::cout << "Invalid vendor.\n";
            return;
    }

    std::string tableName = "webcrawler_" + vendor;
    std::replace(tableName.begin(), tableName.end(), ' ', '_');

    db.createTableWithColumns(tableName,
        {
            {"cve_id",      DataType::STRING},
            {"description", DataType::STRING},
            {"cvss_score",  DataType::DOUBLE},
            {"severity",    DataType::STRING},
            {"cwe_id",      DataType::STRING}
        },
        "cve_id"
    );

    auto ptrs = db.getTablePointers();
    Table* t = db.getTable(tableName);

    auto list = fetchVendorCVEs(vendor, year);

    for (const auto& d : list) {
        t->insertRow(
            {
                d.cve_id,
                d.description,
                formatCVSS(d.cvss_score),
                d.severity,
                d.cwe_id
            },
            ptrs
        );
    }

    std::cout << "Table " << tableName << " created with "
              << list.size() << " enriched CVEs.\n";
}

/**************** Generate ALL Vendor Tables + Combined PDF (Menu option 5) ****************/
void generateAllVendors(Database& db) {

    int year;
    std::cout << "Enter year (e.g., 2026): ";
    std::cin >> year;

    std::vector<std::string> vendors = {
        "MongoDB",
        "MySQL",
        "MariaDB",
        "SAP HANA",
        "Oracle Database",
        "MSSQL",
        "PostgreSQL",
        "IBM DB2"
    };

    auto ptrs = db.getTablePointers();

    for (const auto& vendor : vendors) {

        std::string tableName = "webcrawler_" + vendor;
        std::string safeName = tableName;
        std::replace(safeName.begin(), safeName.end(), ' ', '_');

        db.createTableWithColumns(safeName,
            {
                {"cve_id",      DataType::STRING},
                {"description", DataType::STRING},
                {"cvss_score",  DataType::DOUBLE},
                {"severity",    DataType::STRING},
                {"cwe_id",      DataType::STRING}
            },
            "cve_id"
        );

        Table* t = db.getTable(safeName);

        auto list = fetchVendorCVEs(vendor, year);

        for (const auto& d : list) {
            t->insertRow(
                {
                    d.cve_id,
                    d.description,
                    formatCVSS(d.cvss_score),
                    d.severity,
                    d.cwe_id
                },
                ptrs
            );
        }

        std::cout << "Created table: " << safeName
                  << " (" << list.size() << " CVEs)\n";
    }

    std::cout << "\nGenerating combined PDF...\n";
    exportAllVendorPDF(db);
}
/************************************************************
 * P3.cpp — Main Menu
 * Connects P1 (Database Engine) and P2 (Crawler + PDF Export)
 ************************************************************/

#include <iostream>
#include <string>
#include "P1.cpp"
#include "P2.cpp"

/************************************************************
 * Main Menu
 ************************************************************/
void showMenu() {
    std::cout << "\n==============================\n";
    std::cout << "   PCNC LLC CVE Intelligence\n";
    std::cout << "==============================\n";
    std::cout << "1. List Tables\n";
    std::cout << "2. Display Table\n";
    std::cout << "3. Run Web Crawler (Single Vendor)\n";
    std::cout << "4. Export CVE Report (PDF)\n";
    std::cout << "5. Generate ALL Vendor Tables + Combined PDF\n";
    std::cout << "0. Exit\n";
    std::cout << "==============================\n";
    std::cout << "Choice: ";
}

/************************************************************
 * Main Program
 ************************************************************/
int main() {
    Database db;
    int choice;

    while (true) {
        showMenu();
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Exiting...\n";
            break;
        }

        switch (choice) {

        case 1:
            db.listTables();
            break;

        case 2: {
            std::string tname;
            std::cout << "Enter table name: ";
            std::cin >> tname;

            Table* t = db.getTable(tname);
            if (t) t->display();
            else std::cout << "Table not found.\n";
            break;
        }

        case 3:
            generateWebCrawler(db);
            break;

        case 4:
            exportAllVendorPDF(db);
            break;

        case 5:
            generateAllVendors(db);
            break;

        default:
            std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
