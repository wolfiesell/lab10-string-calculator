#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;


bool isValidDouble(const string& s) {
    if (s.empty()) return false;

    int i = 0;
    if (s[i] == '+' || s[i] == '-') i++;
    if (i >= (int)s.size()) return false;

    bool hasDigitsBeforeDot = false;
    bool hasDigitsAfterDot = false;
    bool hasDot = false;

    for (; i < (int)s.size(); i++) {
        char c = s[i];
        if (isdigit(c)) {
            if (!hasDot)
                hasDigitsBeforeDot = true;
            else
                hasDigitsAfterDot = true;
        } else if (c == '.') {
            if (hasDot) return false; // second dot not allowed
            hasDot = true;
        } else {
            return false;
        }
    }

    if (!hasDot)
        return hasDigitsBeforeDot;

    return hasDigitsBeforeDot && hasDigitsAfterDot;
}

// Remove leading zeros from integer part
string trimLeadingZeros(const string& s) {
    string result = s;
    int i = 0;
    while (i + 1 < (int)result.size() && result[i] == '0')
        i++;
    return result.substr(i);
}

// Equalize decimal lengths
void normalizeDecimals(string &aInt, string &aFrac, string &bInt, string &bFrac) {
    while (aFrac.size() < bFrac.size()) aFrac += '0';
    while (bFrac.size() < aFrac.size()) bFrac += '0';
    while (aInt.size() < bInt.size()) aInt = "0" + aInt;
    while (bInt.size() < aInt.size()) bInt = "0" + bInt;
}

// String-based addition (assuming both positive)
string addStrings(const string& a, const string& b) {
    int carry = 0;
    string result;
    int i = a.size() - 1, j = b.size() - 1;
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += a[i--] - '0';
        if (j >= 0) sum += b[j--] - '0';
        result.push_back('0' + (sum % 10));
        carry = sum / 10;
    }
    reverse(result.begin(), result.end());
    return result;
}

// String-based subtraction (assume a >= b)
string subtractStrings(const string& a, const string& b) {
    int borrow = 0;
    string result;
    int i = a.size() - 1, j = b.size() - 1;
    while (i >= 0) {
        int diff = (a[i] - '0') - borrow - (j >= 0 ? b[j] - '0' : 0);
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else borrow = 0;
        result.push_back('0' + diff);
        i--; j--;
    }
    while (result.size() > 1 && result.back() == '0') result.pop_back();
    reverse(result.begin(), result.end());
    return result;
}

// Add two string-based doubles
string addDoubles(string A, string B) {
    bool negA = false, negB = false;
    if (A[0] == '-') { negA = true; A = A.substr(1); }
    if (A[0] == '+') A = A.substr(1);
    if (B[0] == '-') { negB = true; B = B.substr(1); }
    if (B[0] == '+') B = B.substr(1);

    // Split into integer and fraction
    string aInt = A, aFrac = "", bInt = B, bFrac = "";
    if (A.find('.') != string::npos) {
        aInt = A.substr(0, A.find('.'));
        aFrac = A.substr(A.find('.') + 1);
    }
    if (B.find('.') != string::npos) {
        bInt = B.substr(0, B.find('.'));
        bFrac = B.substr(B.find('.') + 1);
    }

    aInt = trimLeadingZeros(aInt);
    bInt = trimLeadingZeros(bInt);

    normalizeDecimals(aInt, aFrac, bInt, bFrac);

    string sign = "";
    string intResult, fracResult;

    if (negA == negB) {
        // Same sign: add magnitudes
        string fracSum = addStrings(aFrac, bFrac);
        int carry = 0;
        if (fracSum.size() > aFrac.size()) {
            carry = 1;
            fracSum = fracSum.substr(1);
        }
        string intSum = addStrings(aInt, bInt);
        if (carry) intSum = addStrings(intSum, "1");
        intResult = intSum;
        fracResult = fracSum;
        if (negA) sign = "-";
    } else {
        // Opposite signs: perform subtraction
        string Awhole = aInt + aFrac;
        string Bwhole = bInt + bFrac;
        bool aGreater = (Awhole.size() > Bwhole.size()) ||
                        (Awhole.size() == Bwhole.size() && Awhole >= Bwhole);

        string bigInt = aGreater ? aInt : bInt;
        string bigFrac = aGreater ? aFrac : bFrac;
        string smallInt = aGreater ? bInt : aInt;
        string smallFrac = aGreater ? bFrac : aFrac;
        bool resultNeg = aGreater ? negA : negB;

        // Adjust fractional part if needed
        if (smallFrac.size() < bigFrac.size())
            while (smallFrac.size() < bigFrac.size()) smallFrac += '0';

        if (smallFrac > bigFrac) {
            bigFrac = addStrings(bigFrac, "1" + string(bigFrac.size(), '0'));
            bigInt = subtractStrings(bigInt, "1");
        }

        string fracDiff = subtractStrings(bigFrac, smallFrac);
        while (fracDiff.size() < bigFrac.size()) fracDiff = "0" + fracDiff;

        string intDiff = subtractStrings(bigInt, smallInt);
        intResult = intDiff;
        fracResult = fracDiff;
        if (resultNeg) sign = "-";
    }

    // Remove trailing zeros in fraction
    while (!fracResult.empty() && fracResult.back() == '0')
        fracResult.pop_back();

    if (fracResult.empty()) return sign + intResult;
    return sign + intResult + "." + fracResult;
}

// --- Main Program ---
int main() {
    string filename;
    cout << "Enter input file name: ";
    cin >> filename;

    ifstream fin(filename);
    if (!fin) {
        cerr << "Error: Cannot open file.\n";
        return 1;
    }

    string a, b;
    while (fin >> a >> b) {
        cout << "Input: " << a << " , " << b << endl;
        if (!isValidDouble(a)) {
            cout << "Invalid number: " << a << "\n\n";
            continue;
        }
        if (!isValidDouble(b)) {
            cout << "Invalid number: " << b << "\n\n";
            continue;
        }
        string result = addDoubles(a, b);
        cout << "Sum: " << result << "\n\n";
    }
    fin.close();
    return 0;
}
