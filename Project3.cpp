#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <ctime>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;
typedef std::chrono::high_resolution_clock Clock;

class Node;
class BST_Node;
struct Company;
Node* addNodeInner(Node* node, const string& date, double val);
Node* rotateLeft(Node* node);
Node* rotateRight(Node* node);
Node* rotateLeftRight(Node* node);
Node* rotateRightLeft(Node* node);

int getHeight(Node* node);
string returnGreaterDate(string date1, string date2);
BST_Node* addNodeOuter(BST_Node* node, const Company& company);
BST_Node* rotateLeft(BST_Node* node);
BST_Node* rotateRight(BST_Node* node);
BST_Node* rotateLeftRight(BST_Node* node);
BST_Node* rotateRightLeft(BST_Node* node);
BST_Node* rebalance(BST_Node* node);
Node* rebalance(Node* node);
int getHeight(BST_Node* node);
bool isAVL(Node* node);
bool isAVL(BST_Node* node);


struct Company {
    //Added BST/Map by Tab
    Node* entriesBST = nullptr;
    map <string, double> entriesMap;
    string name;
    vector<pair<string, double>> entries;
    Company() { name = ""; }
    explicit Company(string _name) { name = _name; }
};

/*
 * *************************************** File Access ********************************************
 */

// Read takes in CSV file and stores it in a vector of pairs of prices and dates.
// It also reads and stores the treasury bill values that are used in a supporting calculation.
vector<Company*> read(const string& filename, vector<pair<string, double>>& tbills)
{
    ifstream reader;
    string curr, sub, date, tbill;
    double number = 0;
    int index = 0;
    int startPos = 0;
    int comaPos = -1;
    int length;
    bool first = true;
    bool second = true;
    reader.open(filename);
    if (!reader)
    {
        cerr << "Unable to open file";
        exit(2);
    }
    //Deletes data size row
    getline(reader, curr);
    //Names of each company, generate an Object for them.
    getline(reader, curr);
    vector<Company*> companies;
    while (curr.find(',', startPos) != -1) {
        startPos = comaPos + 1;
        comaPos = (int)curr.find(',', startPos);
        length = comaPos - startPos;
        sub = curr.substr(startPos, length);
        if (!sub.empty() && sub != "TBILL") {
            auto* entry = new Company(sub);
            companies.push_back(entry);
        }
    }
    //Takes each row at a time, stores the date, pushes the tbill value to the tbill vector passed in by reference, then creates pairs and adds it to each Company object's vector of entries
    pair<string, double> pairs;
    for (int z = 0; z < 1259; z++) {
        getline(reader, curr);
        first = true;
        second = true;
        startPos = 0;
        comaPos = -1;
        index = 0;
        while (curr.find(",", startPos) != -1) {
            startPos = comaPos + 1;
            comaPos = (int)curr.find(",", startPos);
            length = comaPos - startPos;
            sub = curr.substr(startPos, length);
            if (first == true) {
                //Sets date for pair assignment
                date = sub;
                first = false;
            }
            else if (second == true) {
                //Pushes tbill values to the referenced vector
                sub = sub.substr(0, sub.size() - 1);
                try {
                    if (sub == "#N/") {
                        sub = "0.0";
                    }
                    pair<string, double> pairing;
                    pairing.first = date;
                    pairing.second = stod(sub);
                    tbills.push_back(pairing);
                }
                catch (exception E) {
                    cout << "Error with tbill: " << sub;
                }
                second = false;
            }
            else {
                //Creates pairs of date string and double
                pairs.first = date;
                try {
                    if (sub == "") {
                        sub = "0.0";
                    }
                    pairs.second = stod(sub);
                    companies[index]->entries.push_back(pairs);
                }
                catch (exception E) {
                    cout << "Error in pair stod: " << sub;
                }
                index++;
            }
        }
    }
    return companies;
}

/*
 * ************************ Data Structure 1: Binary Search Tree containing BSTs *****************************
 */

// Note: many of the following BST functions are modified from group member Daniel Sweeney's Project 1 AVL Tree submission.

//Outer BST
class BST_Node {
public:
    BST_Node* left;
    BST_Node* right;
    Company companyData;
    BST_Node() : companyData(nullptr), left(nullptr), right(nullptr) {};
    BST_Node(Company x) : companyData(x), left(nullptr), right(nullptr) {}
    BST_Node(Company x, BST_Node* left, BST_Node* right) : companyData(x), left(left), right(right) {}
};

// Inner BST
class Node {
public:
    Node* left;
    Node* right;
    pair<string, double> data;
    Node() : data("", 0), left(nullptr), right(nullptr) {};
    Node(const string& date, double val) : data(date, val), left(nullptr), right(nullptr) {}
    Node(const string& date, double val, Node* left, Node* right) : data(date, val), left(left), right(right) {}
};

//Inserts a node into the inner BST; the inner BST contains prices and date pairs for one company
Node* insertBSTInner(Node* root, const string& date, double val) {
    if (root == nullptr) {
        Node* insertedElement = new Node(date, val, nullptr, nullptr);
        return insertedElement;
    }
    else {
        return addNodeInner(root, date, val);
    }
}

// Supporting function to insert a node into the inner BST
Node* addNodeInner(Node* node, const string& date, double val) {
    string greaterDate = returnGreaterDate(date, node->data.first);
    if (greaterDate != date) {

        if (node->left == nullptr) {
            Node* insertedElement = new Node(date, val, nullptr, nullptr);
            node->left = insertedElement;

        }

        else {
            addNodeInner(node->left, date, val);
        }
    }

    else if (greaterDate == date) {

        if (node->right == nullptr) {
            Node* insertedElement = new Node(date, val, nullptr, nullptr);
            node->right = insertedElement;

        }
            // Else, recursive call on the right node.
        else {
            addNodeInner(node->right, date, val);
        }
    }
    return node;
}

// AVL rotation functions to balance the inner tree
Node* rotateLeft(Node* node) {
    Node* grandchild = node->right->left;
    Node* newParent = node->right;
    newParent->left = node;
    node->right = grandchild;
    return newParent;
}
Node* rotateRight(Node* node) {
    Node* grandchild = node->left->right;
    Node* newParent = node->left;
    newParent->right = node;
    node->left = grandchild;
    return newParent;
}
Node* rotateLeftRight(Node* node) {
    Node* temp = rotateLeft(node->left);
    node->left = temp;
    Node* newParent = rotateRight(node);
    return newParent;
}
Node* rotateRightLeft(Node* node) {
    Node* temp = rotateRight(node->right);
    node->right = temp;
    Node* newParent = rotateLeft(node);
    return newParent;
}

// AVL rebalance function to rebalance the inner tree
Node* rebalance(Node* node) {
    if (!isAVL(node->left)) {
        node->left = rebalance(node->left);
    }
    if (!isAVL(node->right)) {
        node->right = rebalance(node->right);
    }
    if ((getHeight(node->left) - getHeight(node->right)) > 1) {
        if (getHeight(node->left->left) > getHeight(node->left->right)) {
            node = rotateRight(node);
        }
        else {
            node = rotateLeftRight(node);
        }
    }
    else if ((getHeight(node->right) - getHeight(node->left)) > 1) {
        if (getHeight(node->right->right) > getHeight(node->right->left)) {
            node = rotateLeft(node);
        }
        else {
            node = rotateRightLeft(node);
        }
    }
    return node;
}

// Finds the height of a node by recursively adding to the height until a leaf node is reached.
int getHeight(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    else {
        int heightRight = getHeight(node->right) + 1;
        int heightLeft = getHeight(node->left) + 1;
        if (heightLeft >= heightRight) {
            return getHeight(node->left) + 1;
        }
        else {
            return getHeight(node->right) + 1;
        }
    }
    // Source: I used my solution to Stepik Quiz 5
}

// Tests to see whether the tree is an AVL tree or not.
bool isAVL(Node* node) {
    if (node == nullptr) { return true; }
    if ((getHeight(node->left) - getHeight(node->right) > 1) || (getHeight(node->left) - getHeight(node->right) < -1)) {
        return false;
    }
    else if (!isAVL(node->left)) { return false; }
    else if (!isAVL(node->right)) { return false; }
    else { return true; }
}

// Insert a BST_Node into the outer BST
BST_Node* insertBSTOuter(BST_Node* root, const Company& company) {
    if (root == nullptr) {
        auto* insertedElement = new BST_Node(company, nullptr, nullptr);
        return insertedElement;
    }
    else {
        return addNodeOuter(root, company);
    }
}

// Supporting function to insert a BST_Node into the outer tree
BST_Node* addNodeOuter(BST_Node* node, const Company& company) {

    if (node->companyData.name.compare(company.name) > 0) {
        if (node->left == nullptr) {
            auto* insertedElement = new BST_Node(company, nullptr, nullptr);
            node->left = insertedElement;

        }
        else {
            addNodeOuter(node->left, company);
        }
    }
    else if (node->companyData.name.compare(company.name) < 0) {
        // If there is no node on the right, add the node here and print successful.
        if (node->right == nullptr) {
            auto* insertedElement = new BST_Node(company, nullptr, nullptr);
            node->right = insertedElement;

        }

        else {
            addNodeOuter(node->right, company);
        }
    }
    return node;
}

// Outer tree rotations
BST_Node* rotateLeft(BST_Node* node) {
    BST_Node* grandchild = node->right->left;
    BST_Node* newParent = node->right;
    newParent->left = node;
    node->right = grandchild;
    return newParent;
}
BST_Node* rotateRight(BST_Node* node) {
    BST_Node* grandchild = node->left->right;
    BST_Node* newParent = node->left;
    newParent->right = node;
    node->left = grandchild;
    return newParent;
}
BST_Node* rotateLeftRight(BST_Node* node) {
    BST_Node* temp = rotateLeft(node->left);
    node->left = temp;
    BST_Node* newParent = rotateRight(node);
    return newParent;
}
BST_Node* rotateRightLeft(BST_Node* node) {
    BST_Node* temp = rotateRight(node->right);
    node->right = temp;
    BST_Node* newParent = rotateLeft(node);
    return newParent;
}
// Outer tree function to restore AVL balance
BST_Node* rebalance(BST_Node* node) {
    if (!isAVL(node->left)) {
        node->left = rebalance(node->left);
    }
    if (!isAVL(node->right)) {
        node->right = rebalance(node->right);
    }
    if ((getHeight(node->left) - getHeight(node->right)) > 1) {
        if (getHeight(node->left->left) > getHeight(node->left->right)) {
            node = rotateRight(node);
        }
        else {
            node = rotateLeftRight(node);
        }
    }
    else if ((getHeight(node->right) - getHeight(node->left)) > 1) {
        if (getHeight(node->right->right) > getHeight(node->right->left)) {
            node = rotateLeft(node);
        }
        else {
            node = rotateRightLeft(node);
        }
    }
    return node;
}
// Finds the height of a node by recursively adding to the height until a leaf node is reached (outer tree)
int getHeight(BST_Node* node) {
    if (node == nullptr) {
        return 0;
    }
    else {
        int heightRight = getHeight(node->right) + 1;
        int heightLeft = getHeight(node->left) + 1;
        if (heightLeft >= heightRight) {
            return getHeight(node->left) + 1;
        }
        else {
            return getHeight(node->right) + 1;
        }
    }

}
// Tells if outer tree is a balanced tree or not
bool isAVL(BST_Node* node) {
    if (node == nullptr) { return true; }
    if ((getHeight(node->left) - getHeight(node->right) > 1) || (getHeight(node->left) - getHeight(node->right) < -1)) {
        return false;
    }
    else if (!isAVL(node->left)) { return false; }
    else if (!isAVL(node->right)) { return false; }
    else { return true; }
}

// Builds the outer tree and populates it with company tickers
BST_Node* createBSTOuter(vector<Company*> companies) {
    BST_Node* root = nullptr;
    for (int i = 0; i < companies.size(); i++) {
        root = insertBSTOuter(root, *companies[i]);
        root = rebalance(root);
    }
    return root;
}

// Builds an inner tree (BST contained in BST_Node) of dates/prices for a single company
Node* createBSTInner(Company* company) {
    Node* root = nullptr;
    for (int i = 0; i < company->entries.size(); i++) {
        root = insertBSTInner(root, company->entries[i].first, company->entries[i].second);
        root = rebalance(root);
    }
    return root;
}

/*
 * ************************************** BST Access Functions **************************************
 */

// Returns the date string that is greater (aka more recent date is returned)
string returnGreaterDate(string date1, string date2) {
    string delimiter = "/";
    string month1 = date1.substr(0, date1.find(delimiter));
    string temp1 = date1.substr(date1.find(delimiter) + 1, date1.size());
    string day1 = temp1.substr(0, temp1.find(delimiter));
    string temp_1 = temp1.substr(temp1.find(delimiter) + 1, temp1.size());
    string year1 = temp_1.substr(0, temp_1.find(delimiter));
    int mo1 = stoi(month1);
    int dy1 = stoi(day1);
    int yr1 = stoi(year1);

    string month2 = date2.substr(0, date2.find(delimiter));
    string temp2 = date2.substr(date2.find(delimiter) + 1, date2.size());
    string day2 = temp2.substr(0, temp2.find(delimiter));
    string temp_2 = temp2.substr(temp2.find(delimiter) + 1, temp2.size());
    string year2 = temp_2.substr(0, temp_2.find(delimiter));
    int mo2 = stoi(month2);
    int dy2 = stoi(day2);
    int yr2 = stoi(year2);

    if (yr1 < yr2) {
        return date2;
    }
    else if (yr1 > yr2) {
        return date1;
    }
    else {
        if (mo1 < mo2) {
            return date2;
        }
        else if (mo1 > mo2) {
            return date1;
        }
        else {
            if (dy1 < dy2) {
                return date2;
            }
            else {
                return date1;
            }
        }
    }
}

// Returns a string that is one day prior to the input date
string previousDateString(const string& date) {
    string newDate;
    string delimiter = "/";
    string month = date.substr(0, date.find(delimiter));
    string temp = date.substr(date.find(delimiter) + 1, date.size());
    string day = temp.substr(0, temp.find(delimiter));
    string temp2 = temp.substr(temp.find(delimiter) + 1, temp.size());
    string year = temp2.substr(0, temp2.find(delimiter));
    int mo = stoi(month);
    int dy = stoi(day);
    int yr = stoi(year);
    if (dy != 1) {
        dy = dy - 1;
        day = to_string(dy);
        newDate = month + "/" + day + "/" + year;
        return newDate;
    }
    else {
        dy = 31;
        day = to_string(dy);
        if (mo != 1) {
            mo = mo - 1;
            month = to_string(mo);
            newDate = month + "/" + day + "/" + year;
            return newDate;
        }
        else {
            mo = 12;
            month = to_string(mo);
            yr = yr - 1;
            year = to_string(yr);
            newDate = month + "/" + day + "/" + year;
            return newDate;
        }
    }
}

// Searches inner tree; returns a node with a price/data pair based on a given input date and root node
Node* findDate(Node* root, const string& startDate) {
    // if current node date is bigger than start date, go left; if smaller, go right
    if (root == nullptr) {
        return nullptr;
    }
    if (root->data.first == startDate) {
        return root;
    }
    else if (root->data.first == returnGreaterDate(startDate, root->data.first)) {
        return findDate(root->left, startDate);
    }
    else {
        return findDate(root->right, startDate);
    }
}

// Searches outer tree; returns a root node to the dates and prices for a given company from the outer BST
Node* findCompany(BST_Node* root, const string& ticker) {
    if (root == nullptr) {
        return nullptr;
    }
    else if (root->companyData.name == ticker) {
        Company* ptr = &root->companyData;
        return createBSTInner(ptr);
    }
    else if (root->companyData.name.compare(ticker) < 0) {
        return findCompany(root->right, ticker);
    }
    else {
        return findCompany(root->left, ticker);
    }
}

// Returns the trading day in data source file
string previousDateBST(Node* root, string startDate) {
    string newDate = previousDateString(startDate);
    if (findDate(root, newDate) == nullptr) {
        return previousDateBST(root, newDate);
    }
    else {
        return newDate;
    }
}

// Returns the trading day N trading days before the given date
string goBackNDaysBST(Node* root, string startDate, int numDays) {
    string newDate = startDate;
    for (int i = 0; i < numDays; i++) {
        newDate = previousDateBST(root, newDate);
    }
    return newDate;
}

// Checks to ensure that the input date is within the parameters of our data
bool isValidDate(const string& date) {
    if (date == returnGreaterDate(date, "1/1/2019")) {
        if (date != returnGreaterDate(date, "11/31/2020")) {
            return true;
        }
    }
    return false;
}

/*
 * *********************************** Data Structure 2: Map of Maps ****************************************
 */

// Builds a Map of Company values with tickers as keys
map<string, Company*> createMap(vector<Company*> companies) {
    map <string, Company*> companyMap;
    for (int i = 0; i < companies.size(); i++) {
        companyMap.insert(std::pair<string, Company*>(companies[i]->name, companies[i]));
    }
    return companyMap;
}

// Return previous trading day
string previousDateMap(map<string, double>& prices, const string& startDate) {
    string newDate = previousDateString(startDate);
    if (prices.count(newDate) == 0) {
        newDate = previousDateMap(prices, newDate);
    }
    return newDate;
}

// Returns the trading day N trading days before the given date
string goBackNDaysMap(map<string, double>& prices, string startDate, int numDays) {
    string newDate = startDate;
    for (int i = 0; i < numDays; i++) {
        newDate = previousDateMap(prices, newDate);
    }
    return newDate;
}

// Builds a Map of price values with dates as keys; this map is contained in the Company values of the outer map
map<string, double> createDateMap(Company* company) {
    map <string, double> companyMap;
    for (int i = 0; i < company->entries.size(); i++) {
        companyMap[company->entries[i].first] = company->entries[i].second;
    }
    return companyMap;
}

/*
 * ************************************ Momentum Calculations - BST ************************************
 */

// Finds the simple price return on a stock's price between the start and end dates.
double getReturnBST(Node* companyRoot, const string& startDate, const string& endDate) {
    double oldPrice = findDate(companyRoot, startDate)->data.second;
    double newPrice = findDate(companyRoot, endDate)->data.second;
    // Error check to catch companies that are not in the S&P at that time
    if (oldPrice == 0 || newPrice == 0) {
        return -10000;
    }
    return (newPrice / oldPrice - 1);
}

// Finds the average return of a stock over a given period
double avgReturnBST(Node* companyRoot, string startDate, string endDate) {
    double sum = 0;
    int count = 0;
    string currDate = endDate;
    double temp = 0.0;
    startDate = goBackNDaysBST(companyRoot, startDate, 1);
    while (currDate != startDate) {
        string dateBefore = goBackNDaysBST(companyRoot, currDate, 1);
        temp = getReturnBST(companyRoot, dateBefore, currDate);
        if (temp == -10000) { return -10000; } // Error check for company not in S&P
        sum += temp;
        count++;
        currDate = dateBefore;
    }
    return (sum / count);
}

// Calculates the covariance between a stock and the index over the past year
// Formula: Covariance(stock,index) = SUM((Stock Daily Return - Stock Avg. Period Return) *
// (Index Daily Return - Index Avg. Period Return)) / (Number of days in period - 1)
double covarianceBST(Node* companyRoot, Node* indexRoot, string currDate) {
    int period = 253;   // 253 trading days in a year
    string startDate = goBackNDaysBST(companyRoot, currDate, period - 1);
    double tickerAvgReturn = avgReturnBST(companyRoot, startDate, currDate);
    if (tickerAvgReturn == -10000) { return -10000; } // Error check for company not in S&P
    double indexAvgReturn = avgReturnBST(companyRoot, startDate, currDate);
    // Builds the numerator of the covariance formula
    double numerator = 0.0;
    double valTicker;
    double valIndex;
    startDate = goBackNDaysBST(companyRoot, startDate, 1);
    while (startDate != currDate) {
        string dateBefore = goBackNDaysBST(companyRoot, currDate, 1);
        valTicker = getReturnBST(companyRoot, dateBefore, currDate) - tickerAvgReturn;
        valIndex = getReturnBST(indexRoot, dateBefore, currDate) - indexAvgReturn;
        numerator += (valTicker * valIndex);
        currDate = dateBefore;
    }
    return numerator / (period - 1);
}

// Calculates the 1-Year trailing variance of the market (aka benchmark index)
// Formula: Variance = SUM( (market return - avg. market return)^2) / days in period
double varianceBST(Node* indexRoot, const string& endDate) {
    int period = 253;
    double numerator = 0.0;
    int count = 0;
    string currDate = endDate;
    string startDate = goBackNDaysBST(indexRoot, endDate, period - 1);
    double avgMarketReturn = avgReturnBST(indexRoot, startDate, endDate);
    double marketReturn = 0.0;
    startDate = goBackNDaysBST(indexRoot, startDate, 1);
    while (currDate != startDate) {
        string dateBefore = goBackNDaysBST(indexRoot, currDate, 1);
        marketReturn = getReturnBST(indexRoot, dateBefore, currDate);
        numerator += ((marketReturn - avgMarketReturn) * (marketReturn - avgMarketReturn));
        count++;
        currDate = dateBefore;
    }
    return (numerator / (period - 1));
}

// Calculates the 1-Year trailing beta of a stock
// Formula: Beta = Covariance(stock, market) / Variance(stock)
// Beta is 1-Year Daily Beta
double getBetaBST(Node* companyRoot, Node* indexRoot, const string& date) {
    double numerator = covarianceBST(companyRoot, indexRoot, date);
    if (numerator == -10000) { return -10000; } // Error check for company not in S&P
    double denominator = varianceBST(companyRoot, date);
    return (numerator / denominator);
}

/* Calculates the abnormal or residual return on a stock. This will be used to calculate residual momentum.
 * Abnormal return is the residual return on a stock outside of the expected return and market return
 * This function returns monthly abnormal return
 * Formula: Abnormal return = Stock return - Risk Free Rate - Alpha - [ Beta (benchmark return - risk free rate) ])
 * Risk Free Rate = return on 1-month treasury bill
 * Beta: 1M beta on stock
 * Alpha: Requires statistics beyond the scope of our abilities; however, empirical research shows that alpha
 * does not have a meaningful impact on return in this strategy and can be assumed to be 0.
 * The SPY ETF is used as the benchmark return since it tracks the S&P500 benchmark
*/
double getAbnormalReturnBST(Node* companyRoot, Node* indexRoot, const string& endDate, vector<pair<string, double>>& tBill) {
    string startDate = goBackNDaysBST(companyRoot, endDate, 21);
    double benchmarkReturn = getReturnBST(indexRoot, startDate, endDate);
    double stockReturn = getReturnBST(companyRoot, startDate, endDate);
    double beta = getBetaBST(companyRoot, indexRoot, endDate);
    if (beta == -10000) { return -10000; } // Error check for company not in S&P
    double alpha = 0.0; // assumed
    double riskFreeRate = 0.0;
    for (int i = 0; i < tBill.size(); i++) {
        if (tBill.at(i).first == startDate) {
            riskFreeRate = (tBill.at(i).second) / 100; // Data is stored as %, so divided by 100
            break;
        }
    }
    double expectedReturn = beta * (benchmarkReturn - riskFreeRate);
    double abnormalReturn = stockReturn - riskFreeRate - alpha - expectedReturn;
    return abnormalReturn;
}

// Finds the cumulative sum of monthly abnormal returns from month -12 to month -1
// Research on this strategy recommends omitting the most recent month from the calculation
double cumulativeAbnormalReturnBST(Node* companyRoot, Node* indexRoot, string endDate, vector<pair<string, double>>& tBill) {
    double sumAbnormals = 0.0;
    int numMonths = 11;
    string currDate = goBackNDaysBST(companyRoot, endDate, 21);
    double temp = 0.0;
    for (int i = 0; i < numMonths; i++) {
        temp = getAbnormalReturnBST(companyRoot, indexRoot, currDate, tBill);
        if (temp == -10000) { return -10000; } // Error check for company not in S&P
        sumAbnormals += temp;
        currDate = goBackNDaysBST(companyRoot, currDate, 21);
    }
    return sumAbnormals;
}

// Finds the variance of the monthly residual over the last year (will be used to  calculate residual momentum)
// Formula: Variance = SUM( (abnormal return - avg. abnormal return)^2) / data points
double getAbnormalReturnVarianceBST(Node* companyRoot, Node* indexRoot, const string& endDate, vector<pair<string, double>>& tBill) {
    vector<double> abnormals;
    int numMonths = 11;
    string currDate = goBackNDaysBST(companyRoot, endDate, 21);
    double temp = 0.0;
    double sumAbnormals = 0.0;
    for (int i = 0; i < numMonths; i++) {
        temp = getAbnormalReturnBST(companyRoot, indexRoot, currDate, tBill);
        sumAbnormals += temp;
        abnormals.push_back(temp);
        currDate = goBackNDaysBST(companyRoot, currDate, 21);
    }
    currDate = goBackNDaysBST(companyRoot, endDate, 21);
    double avgAbnormal = sumAbnormals / (abnormals.size());
    double varNumerator = 0.0;
    for (int j = 0; j < abnormals.size(); j++) {
        varNumerator += pow((abnormals.at(j) - avgAbnormal), 2);
        currDate = goBackNDaysBST(companyRoot, currDate, 21);
    }
    return (varNumerator / abnormals.size());
}

// Finds the cumulative sum of monthly variances from month -12 to month -1
double cumulativeVarianceBST(Node* companyRoot, Node* indexRoot, string endDate, vector<pair<string, double>>& tBill) {
    int numMonths = 11;
    double sumVariance = 0.0;
    string currDate = goBackNDaysBST(companyRoot, endDate, 21);
    for (int i = 0; i < numMonths; i++) {
        sumVariance += getAbnormalReturnVarianceBST(companyRoot, indexRoot, currDate, tBill);
        currDate = goBackNDaysBST(companyRoot, currDate, 21);
    }

    return sumVariance;
}

// Driver method. Calculates the residual momentum of a stock and provides an appropriate trade recommendation.
int searchMomentumBST(BST_Node* root, const string& ticker, const string& endDate, vector<pair<string, double>>& tBill) {
    Node* targetRoot = findCompany(root, ticker);
    Node* indexRoot = findCompany(root, "SPY");
    if (targetRoot == nullptr) {
        cout << "Invalid ticker." << endl;
        return -10;
    }
    bool found = false;
    double abnormalReturn = 0.0;
    double stdDeviation = 0.0;  // STD DEV = Sqrt(variance of abnormal return)
    cout << ticker << " found in BST. Calculating momentum..." << endl;
    abnormalReturn = cumulativeAbnormalReturnBST(targetRoot, indexRoot, endDate, tBill);
    if (abnormalReturn == -10000) { // Error check for if company is not in S&P over entire period
        cout << "Company is not in S&P 500 over entire search period. Not enough data is available." << endl;
        return -10;
    }
    stdDeviation = sqrt(cumulativeVarianceBST(targetRoot, indexRoot, endDate, tBill));
    if (abnormalReturn > (stdDeviation * 2)) {
        cout << "Momentum Rating: Strong Positive" << endl;
        cout << "***** Recommendation: Buy in 1 month and hold for 6 months *****" << endl;
        return 2;
    }
    else if (abnormalReturn > stdDeviation) {
        cout << "Momentum Rating: Positive" << endl;
        cout << "***** Recommendation: Buy in 1 month and hold for 6 months *****" << endl;
        return 1;
    }
    else if (abnormalReturn < -(stdDeviation * 2)) {
        cout << "Momentum Rating: Strong Negative" << endl;
        cout << "***** Recommendation: Short in 1 month and close position after 6 months *****" << endl;
        return -2;
    }
    else if (abnormalReturn < -stdDeviation) {
        cout << "Momentum Rating: Negative" << endl;
        cout << "***** Recommendation: Short in 1 month and close position after 6 months *****" << endl;
        return -1;
    }
    else {
        cout << "Momentum Rating: Neutral" << endl;
        cout << "***** Recommendation: Do not trade at this time *****" << endl;
        return 0;
    }
}

/*
 * ************************************ Momentum Calculations - Map *****************************************
 */

// Finds the simple return on a stock's price between the start and end dates.
double getReturnMap(map<string, double>& prices, const string& startDate, const string& endDate) {
    double oldPrice = prices[startDate];
    double newPrice = prices[endDate];
    // Error check to catch companies that are not in the S&P at that time
    if (oldPrice == 0 || newPrice == 0) {
        return -10000;
    }
    double change = (newPrice / oldPrice - 1);
    return change;
}

// Finds the average return of a stock over a given period.
double avgReturnMap(map<string, double>& prices, string startDate, string endDate) {
    double sum = 0;
    int count = 0;
    string currDate = endDate;
    double temp = 0.0;
    startDate = goBackNDaysMap(prices, startDate, 1);
    while (currDate != startDate) {
        string dateBefore = goBackNDaysMap(prices, currDate, 1);
        temp = getReturnMap(prices, dateBefore, currDate);
        if (temp == -10000) { return -10000; } // Error check for company not in S&P
        sum += temp;
        count++;
        currDate = dateBefore;
    }
    return (sum / count);
}

// Note: Covariance(stock,index) = SUM((Stock Daily Return - Stock Avg. Period Return) *
// (Index Daily Return - Index Avg. Period Return)) / (Number of days in period - 1)
double covarianceMap(map<string, double>& prices, map<string, double>& indexPrices, string currDate) {
    int period = 253;   // 253 trading days in a year
    string startDate = goBackNDaysMap(prices, currDate, period - 1);
    double tickerAvgReturn = avgReturnMap(prices, startDate, currDate);
    if (tickerAvgReturn == -10000) { return -10000; } // Error check for company not in S&P
    double indexAvgReturn = avgReturnMap(prices, startDate, currDate);
    // Builds the numerator of the covariance formula
    double numerator = 0.0;
    double valTicker;
    double valIndex;
    startDate = goBackNDaysMap(prices, startDate, 1);
    while (startDate != currDate) {
        string dateBefore = goBackNDaysMap(prices, currDate, 1);
        valTicker = getReturnMap(prices, dateBefore, currDate) - tickerAvgReturn;
        valIndex = getReturnMap(indexPrices, dateBefore, currDate) - indexAvgReturn;
        numerator += (valTicker * valIndex);
        currDate = dateBefore;
    }
    // Covariance Formula
    return numerator / (period - 1);
}

// Calculates the 1-Year trailing variance of the market
// Formula: Variance = SUM( (market return - avg. market return)^2) / days in period
double varianceMap(map<string, double>& indexPrices, const string& endDate) {
    int period = 253;
    double numerator = 0.0;
    int count = 0;
    string currDate = endDate;
    string startDate = goBackNDaysMap(indexPrices, endDate, period - 1);
    double avgMarketReturn = avgReturnMap(indexPrices, startDate, endDate);
    double marketReturn = 0.0;
    startDate = goBackNDaysMap(indexPrices, startDate, 1);
    while (currDate != startDate) {
        string dateBefore = goBackNDaysMap(indexPrices, currDate, 1);
        marketReturn = getReturnMap(indexPrices, dateBefore, currDate);
        numerator += ((marketReturn - avgMarketReturn) * (marketReturn - avgMarketReturn));
        currDate = dateBefore;
    }
    return (numerator / (period - 1));
}

// Calculates the 1-Year trailing beta of a stock
// Beta = Covariance(stock, market) / Variance(stock)
// Beta is 1-Year Daily Beta
double getBetaMap(map<string, double>& prices, map<string, double>& indexPrices, const string& date) {
    double numerator = covarianceMap(prices, indexPrices, date);
    if (numerator == -10000) { return -10000; } // Error check for if company is not in S&P over period
    double denominator = varianceMap(prices, date);
    return (numerator / denominator);
}

/* Abnormal return is the residual return on a stock outside of the expected return and market return
 * This function returns monthly abnormal return
 * Formula: Abnormal return = Stock return - Risk Free Rate - Alpha - [ Beta (benchmark return - risk free rate) ])
*/
double getAbnormalReturnMap(map<string, double>& prices, map<string, double>& indexPrices, const string& endDate, vector<pair<string, double>>& tBill) {
    string startDate = goBackNDaysMap(prices, endDate, 21);
    double benchmarkReturn = getReturnMap(indexPrices, startDate, endDate);
    double stockReturn = getReturnMap(prices, startDate, endDate);
    double beta = getBetaMap(prices, indexPrices, endDate);
    if (beta == -10000) { return -10000; } // Error check for if company is not in S&P over period
    double alpha = 0.0; // assumed
    double riskFreeRate = 0.0;
    for (int i = 0; i < tBill.size(); i++) {
        if (tBill.at(i).first == startDate) {
            riskFreeRate = (tBill.at(i).second) / 100; // Data is stored as %, so divided by 100
            break;
        }
    }
    double expectedReturn = beta * (benchmarkReturn - riskFreeRate);
    double abnormalReturn = stockReturn - riskFreeRate - alpha - expectedReturn;
    /*cout << "S&P Return: " << benchmarkReturn << endl;
    cout << ticker << ": " << stockReturn << endl;
    cout << "Beta: " << beta << endl;
    cout << "Risk Free Rate: " << riskFreeRate << endl;
    cout << "Expected Return: " << expectedReturn << endl;
    cout << "Ab Return: " << abnormalReturn << endl; */
    return abnormalReturn;
}

// Finds the cumulative sum of monthly abnormal returns from month -12 to month -1
// Research on this strategy recommends omitting the most recent month from the calculation
double cumulativeAbnormalReturnMap(map<string, double>& prices, map<string, double>& indexPrices, string endDate, vector<pair<string, double>>& tBill) {
    // Finds the cumulative sum of monthly abnormal returns from month -12 to month -1
    double sumAbnormals = 0.0;
    int numMonths = 11;
    string currDate = goBackNDaysMap(prices, endDate, 21);  // do not include most recent month
    double temp = 0.0;
    for (int i = 0; i < numMonths; i++) {
        temp = getAbnormalReturnMap(prices, indexPrices, currDate, tBill);
        if (temp == -10000) { return -10000; } // Error check for if company is not in S&P over period
        sumAbnormals += temp;
        currDate = goBackNDaysMap(prices, currDate, 21);
        temp = 0.0;
    }
    return sumAbnormals;
}

// Finds the variance of the monthly residual over the last year (will be used to  calculate residual momentum)
// Formula: Variance = SUM( (abnormal return - avg. abnormal return)^2) / data points
double getAbnormalReturnVarianceMap(map<string, double>& prices, map<string, double>& indexPrices, const string& endDate, vector<pair<string, double>>& tBill) {
    // Finds the variance of the monthly residual over the last year
    // Formula: Variance = SUM( (abnormal return - avg. abnormal return)^2) / data points
    vector<double> abnormals;
    int numMonths = 11;
    string currDate = goBackNDaysMap(prices, endDate, 21);
    double temp = 0.0;
    double sumAbnormals = 0.0;
    for (int i = 0; i < numMonths; i++) {
        temp = getAbnormalReturnMap(prices, indexPrices, currDate, tBill);
        sumAbnormals += temp;
        abnormals.push_back(temp);
        currDate = goBackNDaysMap(prices, currDate, 21);
    }
    currDate = goBackNDaysMap(prices, endDate, 21);
    double avgAbnormal = sumAbnormals / (abnormals.size());
    double varNumerator = 0.0;
    for (double abnormal : abnormals) {
        varNumerator += pow((abnormal - avgAbnormal), 2);
        currDate = goBackNDaysMap(prices, currDate, 21);
    }
    return (varNumerator / abnormals.size());
}

// Finds the cumulative sum of monthly variances from month -12 to month -1
double cumulativeVarianceMap(map<string, double>& prices, map<string, double>& indexPrices, string endDate, vector<pair<string, double>>& tBill) {
    int numMonths = 11;
    double sumVariance = 0.0;
    string currDate = goBackNDaysMap(prices, endDate, 21);
    for (int i = 0; i < numMonths; i++) {
        sumVariance += getAbnormalReturnVarianceMap(prices, indexPrices, currDate, tBill);
        currDate = goBackNDaysMap(prices, currDate, 21);
    }
    return sumVariance;
}

// Driver method. Calculates the residual momentum of a stock and provides an appropriate trade recommendation.
int searchMomentumMap(map<string, double>& prices, map<string, double>& indexPrices, const string& ticker, const string& endDate, vector<pair<string, double>>& tBill) {
    double abnormalReturn = 0.0;
    double stdDeviation = 0.0;  // STD DEV = Sqrt(variance of abnormal return)
    cout << ticker << " found in Map. Calculating momentum..." << endl;
    abnormalReturn = cumulativeAbnormalReturnMap(prices, indexPrices, endDate, tBill);
    if (abnormalReturn == -10000) { // Error check for if company is not in S&P over entire period
        cout << "Company is not in S&P 500 over entire search period. Not enough data is available." << endl;
        return -10;
    }
    stdDeviation = sqrt(cumulativeVarianceMap(prices, indexPrices, endDate, tBill));
    if (abnormalReturn > (stdDeviation * 2)) {
        cout << "Momentum Rating: Strong Positive" << endl;
        cout << "***** Recommendation: Buy in 1 month and hold for 6 months *****" << endl;
        return 2;
    }
    else if (abnormalReturn > stdDeviation) {
        cout << "Momentum Rating: Positive" << endl;
        cout << "***** Recommendation: Buy in 1 month and hold for 6 months *****" << endl;
        return 1;
    }
    else if (abnormalReturn < -(stdDeviation * 2)) {
        cout << "Momentum Rating: Strong Negative" << endl;
        cout << "***** Recommendation: Short in 1 month and close position after 6 months *****" << endl;
        return -2;
    }
    else if (abnormalReturn < -stdDeviation) {
        cout << "Momentum Rating: Negative" << endl;
        cout << "***** Recommendation: Short in 1 month and close position after 6 months *****" << endl;
        return -1;
    }
    else {
        cout << "Momentum Rating: Neutral" << endl;
        cout << "***** Recommendation: Do not trade at this time *****" << endl;
        return 0;
    }
}


//  ************************************ Output Code ****************************************

int main()
{
    cout << "***************************************************\n";
    cout << "Welcome to the Get Rich or Get Poor Trying Program!\n";
    cout << "***************************************************\n";
    cout << "Initializing data...";

    vector<pair<string, double>> tbill;
    string outputString = "Ticker Momentum Ratings:\n";
    //This will store all companies from the data.
    vector<Company*> companies = read("Project3DataNew.csv", tbill);

    bool validDate = false;
    cout << "loaded. Thank you for your patience!\n\n";
    cout << "This program calculates a stock's idiosyncratic momentum on a given date and provides a recommendation on how to invest.\n" << endl;
    string date;
    while (!validDate) {
        cout << "On what date would you like to know your stock's momentum? (Please enter a date between 01/01/2019 and 11/30/2020 (MM/DD/YYYY): ";
        cin >> date;
        try {
            if (isValidDate(date))
                validDate = true;
            else
                cout << "Sorry, that is not a valid date.\n";
            continue;
        }
        catch (exception E) {
            cout << "Sorry, that is not a valid date.\n";
            continue;
        }
    }
    //Iterates until the user is done checking tickers.
    bool done = false;
    int momentumRating = 0;
    bool uppercaseTicker;
    string ticker;
    bool tickerUsed = false;
    auto start = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<chrono::milliseconds>(end - start);

    while (!done) {
        tickerUsed = false;
        uppercaseTicker = false;
        //Uppercases ticker
        while (!uppercaseTicker) {
            cout << "What stock would you like to look up? Enter ticker: ";
            cin >> ticker;
            try {
                std::transform(ticker.begin(), ticker.end(), ticker.begin(), ::toupper);
                uppercaseTicker = true;
            }
            catch (exception E) {
                cout << "Invalid ticker.\n";
            }
        }
        string choice;
        //Selects which data structure to use
        cout << "Please designate which data structure to use (BST/MAP/BOTH): ";
        cin >> choice;
        try {
            std::transform(choice.begin(), choice.end(), choice.begin(), ::toupper);
        }
        catch (exception E) {
            cout << "Invalid choice.\n";
        }
        if (choice == "BST") {
            start = high_resolution_clock::now();
            //Creates BST tree
            BST_Node* BST_root = createBSTOuter(companies);
            //BST assessment
            momentumRating = searchMomentumBST(BST_root, ticker, date, tbill);
            end = high_resolution_clock::now();
            duration = duration_cast<chrono::milliseconds>(end - start);
            cout << "Total BST Time: " << duration.count() << " milliseconds\n";
            cout << endl;
            tickerUsed = true;
        }
        else if(choice == "MAP") {
            start = high_resolution_clock::now();
            map <string, Company*> outerMap = createMap(companies);
            map <string, double> indexPrices = createDateMap(outerMap["SPY"]);
            if (outerMap.count(ticker) == 0) {
                cout << "Invalid ticker." << endl;
            }
            else {
                map<string, double> prices = createDateMap(outerMap[ticker]);
                searchMomentumMap(prices, indexPrices, ticker, date, tbill);
                end = high_resolution_clock::now();
                duration = duration_cast<chrono::milliseconds>(end - start);
                cout << "Total Map Time: " << duration.count() << " milliseconds\n";
                cout << endl;
                tickerUsed = true;
            }
        }
        else if(choice == "BOTH") {
            start = high_resolution_clock::now();
            map <string, Company*> outerMap = createMap(companies);
            map <string, double> indexPrices = createDateMap(outerMap["SPY"]);
            if (outerMap.count(ticker) == 0) {
                cout << "Invalid ticker." << endl;
            }
            else {
                map<string, double> prices = createDateMap(outerMap[ticker]);
                searchMomentumMap(prices, indexPrices, ticker, date, tbill);
                end = high_resolution_clock::now();
                duration = duration_cast<chrono::milliseconds>(end - start);
                cout << "Total Map Time: " << duration.count() << " milliseconds\n";
                cout << endl;
                start = high_resolution_clock::now();
                //Creates BST tree
                BST_Node* BST_root = createBSTOuter(companies);
                //BST assessment
                momentumRating = searchMomentumBST(BST_root, ticker, date, tbill);
                end = high_resolution_clock::now();
                duration = duration_cast<chrono::milliseconds>(end - start);
                cout << "Total BST Time: " << duration.count() << " milliseconds\n";
                cout << endl;
                tickerUsed = true;
            }
        }
        else {
            cout << "Please enter a valid data structure choice.\n";
        }
        bool valid = false;
        if (momentumRating != -10 && tickerUsed == true) {
            //-10 is error code, all other values are valid write conditions
            valid = false;
            while (!valid)
            {
                string writeFile;
                cout << "Would you like to save this result to file? (Y/N): ";
                cin >> writeFile;
                cout << "\n";

                try {
                    std::transform(writeFile.begin(), writeFile.end(), writeFile.begin(), ::toupper);
                }
                catch (exception E) {
                    cout << "Please enter Y or N.\n";
                }

                if (writeFile == "Y" || writeFile == "YES") {
                    //Writes the ticker and the correlation to file.
                    ofstream writer("Output.txt");
                    if (!writer) {
                        cout << "Failed to open write file\n";
                    }
                    outputString += ticker + " Correlation: ";
                    switch (momentumRating) {
                        case -2:
                            outputString += "Strong Negative\n";
                            break;
                        case -1:
                            outputString += "Negative\n";
                            break;
                        case 0:
                            outputString += "Neutral\n";
                            break;
                        case 1:
                            outputString += "Positive\n";
                            break;
                        case 2:
                            outputString += "Strong Positive\n";
                            break;
                        default:
                            break;
                    }
                    writer << outputString;
                    valid = true;
                }
                else if (writeFile == "N" || writeFile == "NO") {
                    //Exits loop
                    valid = true;
                }
                else {
                    cout << "Please enter Y or N.\n";
                }
            }
        }
        valid = false;
        while (!valid)
        {
            string goAgain;
            cout << "Would you like to perform another operation? (Y/N): ";
            cin >> goAgain;
            try {
                std::transform(goAgain.begin(), goAgain.end(), goAgain.begin(), ::toupper);
            }
            catch (exception E) {
                cout << "Please enter Y or N.\n";
            }
            cout << "\n";
            if (goAgain == "Y" || goAgain == "YES") {
                done = false;
                valid = true;
            }
            else if (goAgain == "N" || goAgain == "NO") {
                //Exits loop
                valid = true;
                done = true;
            }
            else {
                cout << "Please enter Y or N.\n";
            }
        }
    }
    cout << "Thank you for trying to get rich. May the odds ever be in your favor!\n";
    return 0;
}