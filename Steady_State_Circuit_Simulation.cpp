#include<iostream>

using namespace std;


//  INPUT MODULE


void inputTopology(int e, int* SN, int* EN) {
    cout << "\nStarting Node >> Ending Node (for each element):" << endl;
    for (int i = 0; i < e; i++) {
        cin >> SN[i] >> EN[i];
    }
}

void inputGeMatrix(int e, double ge[][50]) {
    cout << "\nENTER GE MATRIX (admittance diagonal values):" << endl;
    for (int i = 0; i < e; i++)
        for (int j = 0; j < e; j++)
            ge[i][j] = (i == j) ? (cin >> ge[i][j], ge[i][j]) : 0;
}

void inputVse(int e, double Vse[][1]) {
    cout << "\nENTER VSE VALUES (source voltage for each element):" << endl;
    for (int i = 0; i < e; i++)
        cin >> Vse[i][0];
}

void inputIse(int e, double ise[][1]) {
    cout << "\nENTER ISE VALUES (source current for each element):" << endl;
    for (int i = 0; i < e; i++)
        cin >> ise[i][0];
}


//  MATRIX CONSTRUCTION MODULE


// Build element-node incidence matrix A (e x n)
// and reduced A matrix (n-1 x e) and A transpose (e x n-1)
void buildIncidenceMatrix(int e, int n, int* SN, int* EN,
                          double A_matrix[][50], double A_transpose[][50]) {
    int A[50][50] = {};
    for (int i = 0; i < e; i++) {
        for (int j = 0; j < n; j++) {
            if      (SN[i] == j) A[i][j] =  1;
            else if (EN[i] == j) A[i][j] = -1;
            else                 A[i][j] =  0;
        }
    }

    // Print
    cout << "\nA matrix (reduced):" << endl;
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < e; i++) {
            A_transpose[i][j - 1] = A[i][j];
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

    // A_matrix [n-1 x e]  A_transpose
    for (int i = 0; i < e; i++)
        for (int j = 0; j < n - 1; j++)
            A_matrix[j][i] = A_transpose[i][j];
}


//  NODAL ADMITTANCE MODULE


// Compute Yn = [A][ge][A']
void computeYn(int e, int n, double A_matrix[][50], double ge[][50],
               double A_transpose[][50], double Yn[][50]) {
    double mult[50][50] = {}; // mult = [A][ge]

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < e; j++)
            for (int k = 0; k < e; k++)
                mult[i][j] += A_matrix[i][k] * ge[k][j];

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1; j++)
            for (int k = 0; k < e; k++)
                Yn[i][j] += mult[i][k] * A_transpose[k][j];

    cout << "\n[Yn] = [A][ge][A'] :" << endl;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1; j++)
            cout << Yn[i][j] << " ";
        cout << endl;
    }
}


//  SOURCE CURRENT MODULE


// Compute Isn = [A][ge][Vse] - [A][ise]
void computeIsn(int e, int n, double A_matrix[][50], double ge[][50],
                double Vse[][1], double ise[][1], double Isn[][1]) {
    double mult[50][50] = {}; // [A][ge]
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < e; j++)
            for (int k = 0; k < e; k++)
                mult[i][j] += A_matrix[i][k] * ge[k][j];

    double mult3[50][1] = {}; // [A][ge][Vse]
    for (int i = 0; i < n - 1; i++)
        for (int k = 0; k < e; k++)
            mult3[i][0] += mult[i][k] * Vse[k][0];

    double mult4[50][1] = {}; // [A][ise]
    for (int i = 0; i < n - 1; i++)
        for (int k = 0; k < e; k++)
            mult4[i][0] += A_matrix[i][k] * ise[k][0];

    for (int i = 0; i < n - 1; i++)
        Isn[i][0] = mult3[i][0] - mult4[i][0];

    cout << "\nIsn = [A][ge][Vse] - [A][ise]:" << endl;
    for (int i = 0; i < n - 1; i++)
        cout << Isn[i][0] << endl;
}


//  MATRIX INVERSE MODULE 


void invertMatrix(int n, double Yn[][50], double I[][50]) {
    // Initialise I as identity matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            I[i][j] = (i == j) ? 1.0 : 0.0;

  
    for (int i = 0; i < n; i++) {
        double a = Yn[i][i];
        for (int j = 0; j < n; j++) {
            Yn[i][j] /= a;
            I[i][j]  /= a;
        }
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            double b = Yn[j][i];
            for (int k = 0; k < n; k++) {
                Yn[j][k] -= b * Yn[i][k];
                I[j][k]  -= b * I[i][k];
            }
        }
    }
}


//  NODE VOLTAGE MODULE


// Compute Vn = [Yn]^-1 * Isn
void computeVn(int n, double YnInv[][50], double Isn[][1], double Vn[][1]) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1; j++)
            Vn[i][0] += YnInv[i][j] * Isn[j][0];

    cout << "\nVn (node voltages):" << endl;
    for (int i = 0; i < n - 1; i++)
        cout << "V" << (i + 1) << " = " << Vn[i][0] << " V" << endl;
}


//  ELEMENT VOLTAGE MODULE


// Compute Ve = [A']* Vn
void computeVe(int e, int n, double A_transpose[][50],
               double Vn[][1], double Ve[][1]) {
    for (int i = 0; i < e; i++)
        for (int k = 0; k < n - 1; k++)
            Ve[i][0] += A_transpose[i][k] * Vn[k][0];

    cout << "\nVe (element voltages):" << endl;
    for (int i = 0; i < e; i++)
        cout << "Ve[" << (i + 1) << "] = " << Ve[i][0] << " V" << endl;
}


//  ELEMENT CURRENT MODULE


// Compute ie = [ge](Ve - Vse) + ise
void computeIe(int e, double ge[][50], double Ve[][1],
               double Vse[][1], double ise[][1], double ie[][1]) {
    double S[50][1] = {}; // S = Ve - Vse
    double P[50][1] = {}; // P = ge * S

    for (int i = 0; i < e; i++)
        S[i][0] = Ve[i][0] - Vse[i][0];

    for (int i = 0; i < e; i++)
        for (int j = 0; j < e; j++)
            P[i][0] += ge[i][j] * S[j][0];

    for (int i = 0; i < e; i++)
        ie[i][0] = P[i][0] + ise[i][0];

    cout << "\nie (element currents):" << endl;
    for (int i = 0; i < e; i++)
        cout << "ie[" << (i + 1) << "] = " << ie[i][0] << " A" << endl;
}

//  MAIN


int main() {
    int e, n;

    cout << "===== STEADY STATE CIRCUIT SIMULATION =====" << endl;
    cout << "Enter number of elements: "; cin >> e;
    cout << "Enter number of nodes   : "; cin >> n;

    // matrix declaration
    int    SN[50], EN[50];
    double ge[50][50]          = {};
    double Vse[50][1]          = {};
    double ise[50][1]          = {};
    double A_matrix[50][50]    = {};
    double A_transpose[50][50] = {};
    double Yn[50][50]          = {};
    double YnInv[50][50]       = {};
    double Isn[50][1]          = {};
    double Vn[50][1]           = {};
    double Ve[50][1]           = {};
    double ie[50][1]           = {};

    
    inputTopology(e, SN, EN);
    inputGeMatrix(e, ge);
    inputVse(e, Vse);
    inputIse(e, ise);

    buildIncidenceMatrix(e, n, SN, EN, A_matrix, A_transpose);
    computeYn(e, n, A_matrix, ge, A_transpose, Yn);
    computeIsn(e, n, A_matrix, ge, Vse, ise, Isn);

    invertMatrix(n - 1, Yn, YnInv);

    computeVn(n, YnInv, Isn, Vn);
    computeVe(e, n, A_transpose, Vn, Ve);
    computeIe(e, ge, Ve, Vse, ise, ie);

    return 0;
}