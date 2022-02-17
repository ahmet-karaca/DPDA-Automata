#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

using namespace std;


bool isInList(const string& item, const vector<string>& list);
void executer(vector<string> &currentStateVector, stack<string> &stack,string &currentState, int &inputNumber, bool &finisher, ofstream &outputFile);
string stackPrinter(stack<string> stack);

vector<string> stateFinder(const string &currentState, const string &inputStr, const string &stackTopStr, vector<vector<string> > &partTVector);

vector<string> split2(const string& text);

int main(int argc, char** argv) {

    // alınan dpdaFile
    ifstream dpdaFile;
    dpdaFile.open(argv[1]);
    string line1;

    // dpdaFile den alınan verilerle doldurulacak dpdaVector
    vector<string> dpdaVector;

    // butun line1 lari tek tek okuyup dpdaVector e ekliyor
    while (getline(dpdaFile, line1)){
        dpdaVector.push_back(line1);
    }

    dpdaFile.close();

    // burada outputFile i da olusturuyor
    ofstream outputFile;
    outputFile.open(argv[3]);

    // burada partQ yu isliyor initial ve final state leri olusturuyor
    string partQ = dpdaVector[0];
    int ilkbosluk = partQ.find(" ",0);
    string ilkpart = partQ.substr(2,ilkbosluk-2);
    vector<string> ilkpartVector = split2(ilkpart);
    string ikincipart = partQ.substr(ilkbosluk+4,partQ.size()-(ilkbosluk+4));
    int ilkKapaParantez = ikincipart.find(")",0);
    string initialState = ikincipart.substr(1,ilkKapaParantez-1);
    vector<string> finalStates;
    string finalStatesStr = ikincipart.substr(ilkKapaParantez+2,ikincipart.size());
    int koseliAcParantez = 0;
    int koseliKapaParantez = 0;
    koseliAcParantez = finalStatesStr.find("[",0);
    koseliKapaParantez = finalStatesStr.find("]",0);

    while (koseliAcParantez != -1){
        string tempFinalStateStr = finalStatesStr.substr(koseliAcParantez+1,koseliKapaParantez-(koseliAcParantez+1));
        finalStates.push_back(tempFinalStateStr);
        koseliAcParantez = finalStatesStr.find("[",koseliAcParantez+1);
        koseliKapaParantez = finalStatesStr.find("]", koseliKapaParantez + 1);
    }

    // burada partA yi isliyor ve partAVector u olusturuyor
    string partA = dpdaVector[1];
    partA = partA.substr(2,partA.size()-2);
    vector<string> partAVector = split2(partA);

    // burada partZ yi isliyor ve partZVector u olusturuyor
    string partZ = dpdaVector[2];
    partZ = partZ.substr(2,partZ.size()-2);
    vector<string> partZVector = split2(partZ);

    // burada partT yi isliyor ve partTVector u olusturuyor
    vector<vector<string> > partTVector;
    vector<string> initialStateVector;

    for (int i = 3; i < dpdaVector.size(); ++i) {
        string partT = dpdaVector[i];
        partT = partT.substr(2,partT.size());
        vector<string> temp = split2(partT);
        if (temp[0] == initialState){
            initialStateVector.push_back(temp[0]);
            initialStateVector.push_back(temp[1]);
        }
        partTVector.push_back(temp);
    }

    // burada partA ve partZ ye "e" karakterini ekliyor
    partAVector.push_back("e");
    partZVector.push_back("e");

    // burada partT yi invalid character var mı diye kontrol ediyor
    for (int i = 0; i <partTVector.size(); ++i) {
        bool bool0 = isInList(partTVector[i][0], ilkpartVector);

        bool bool1 = isInList(partTVector[i][1], partAVector);

        bool bool2 = isInList(partTVector[i][2], partZVector);

        bool bool3 = isInList(partTVector[i][3], ilkpartVector);

        bool bool4 = isInList(partTVector[i][4], partZVector);

        // eger invalid character varsa bool false oluyor ve if e giriyor
        if (!bool0  | !bool1 | !bool2 | !bool3 | !bool4){
            // outputFile a error mesajini yaziyor ve programi kapatiyor
            outputFile << "Error [1]:DPDA description is invalid!" << endl;
            exit(0);
        }

    }


    // alınan dpdaInputFile
    ifstream dpdaInputFile;
    dpdaInputFile.open(argv[2]);
    string line2;


    // input verilerini depolayan vector
    vector<vector<string> > dpdaInputVector;

    // burada butun line2 lari tek tek okuyup her line i , den split edip dpdaInputVector e vector olarak ekliyor
    while (getline(dpdaInputFile, line2)){
        vector<string> tempVector = split2(line2);
        dpdaInputVector.push_back(tempVector);
    }

    dpdaInputFile.close();


    // inputFile daki satir sayisi kadar for dongusu donuyor
    for (int i = 0; i < dpdaInputVector.size(); ++i) {
        stack<string> stack;
        bool finisher = false;
        int inputNumber = 0;
        string currentState;

        // initialstatin while disinda yapilmasi
        // su anki state i iceren vector
        vector<string> currentStateVector;
        // su anki satir bossa gerekli islemlere basliyor
        if (dpdaInputVector[i].empty()){
            inputNumber = -1;
            currentStateVector = stateFinder(initialStateVector[0], "e", "e", partTVector);
        }
        else{
            currentStateVector = stateFinder(initialStateVector[0], initialStateVector[1], "e", partTVector);

            outputFile << currentStateVector[0] << "," << currentStateVector[1] << "," << currentStateVector[2] << " => " << currentStateVector[3] << "," << currentStateVector[4] << " ";

            executer(currentStateVector, stack, currentState, inputNumber, finisher, outputFile);

            outputFile << stackPrinter(stack) << endl;
        }

        while (!finisher){
            if (currentStateVector.size() == 1){
                // gecerli state bulamadi
                // accept mi reject mi diye kontrol eder ve outputFile a yazar
                // donguyu kirar varsa yeni inputa gecer

                bool isAccept = false;
                for (int j = 0; j <finalStates.size(); ++j) {
                    if (currentStateVector[0] == finalStates[j]){
                        if (stack.empty()){
                            isAccept = true;
                        }
                        else if (stack.size() == 1){
                            if (stack.top() == "$"){
                                isAccept = true;
                            }
                        }
                    }
                }
                if (isAccept){
                    outputFile << "ACCEPT" << endl;
                    outputFile << endl;
                }
                else{
                    outputFile << "REJECT" << endl;
                    outputFile << endl;
                }
                break;
            }

            if (inputNumber >= dpdaInputVector[i].size()){
                // inputNumber satirdaki veri sayisindan buyuk
                // artik inputStr hep "e" olacaktir
                if (stack.empty()){
                    currentStateVector = stateFinder(currentState, "e", "e", partTVector);
                }
                else{
                    currentStateVector = stateFinder(currentState, "e", stack.top(), partTVector);
                }
            }
            else if(inputNumber == -1){
                // inputNumber -1 cunku inputFile daki satir bos
                currentStateVector = stateFinder(currentState, "e", stack.top(), partTVector);
            }
            else{
                currentStateVector = stateFinder(currentState, dpdaInputVector[i][inputNumber], stack.top(), partTVector);
            }

            if (currentStateVector.size() == 1){
                // gecerli state bulamadi
                // accept mi reject mi diye kontrol eder ve outputFile a yazar
                // donguyu kirar varsa yeni inputa gecer

                bool isAccept = false;
                for (int j = 0; j <finalStates.size(); ++j) {
                    if (currentStateVector[0] == finalStates[j]){
                        if (stack.empty()){
                            isAccept = true;
                        }
                        else if (stack.size() == 1){
                            if (stack.top() == "$"){
                                isAccept = true;
                            }
                        }
                    }
                }
                if (isAccept){
                    outputFile << "ACCEPT" << endl;
                    outputFile << endl;
                }
                else{
                    outputFile << "REJECT" << endl;
                    outputFile << endl;
                }
                break;
            }

            executer(currentStateVector, stack, currentState, inputNumber, finisher, outputFile);

            outputFile << currentStateVector[0] << "," << currentStateVector[1] << "," << currentStateVector[2] << " => " << currentStateVector[3] << "," << currentStateVector[4] << " ";

            outputFile << stackPrinter(stack) << endl;
        }
    }

    outputFile.close();
    return 0;
}

// executer fonksiyonu su anki state i alir ve sirasiyla yapilmasi gereken islemleri yapar
void executer(vector<string> &currentStateVector, stack<string> &stack,string &currentState, int &inputNumber, bool &finisher, ofstream &outputFile){

    // input kismi "e" dir. inputNumber degismez ayni kalir
    if (currentStateVector[1] == "e"){
        // pop
        if (currentStateVector[2] == "e"){
            // "e" oldugu icin pop islemi yapilmadi
        }
        else{
            // stack.top() a bakar. eger pop elemani ile ayniysa devam edilir degilse hata vardir reject edilir
            if (stack.top() == currentStateVector[2]){
                stack.pop();
            }
            else{
                finisher = true;
                outputFile << "REJECT";
                return;
            }
        }

        // push
        if (currentStateVector[4] == "e"){
            // "e" oldugu icin push islemi yapilmadi
        }
        else{
            stack.push(currentStateVector[4]);
        }

        // currentState yeni degerini alir
        currentState = currentStateVector[3];
    }
    else{
        // input inputFile dan alindi

        // pop
        if (currentStateVector[2] == "e"){
            // "e" oldugu icin pop islemi yapilmadi
        }
        else{
            // stack.top() a bakar. eger pop elemani ile ayniysa devam edilir degilse hata vardir reject edilir
            if (stack.top() == currentStateVector[2]){
                stack.pop();
            }
            else{
                finisher = true;
                outputFile << "REJECT";
                return;
            }
        }

        // push
        if (currentStateVector[4] == "e"){
            // "e" oldugu icin push islemi yapilmadi
        }
        else{
            stack.push(currentStateVector[4]);
        }

        // currentState yeni degerini alir
        currentState = currentStateVector[3];

        if (inputNumber != -1){
            inputNumber++;
        }
    }
}

// verilen currentState, inputStr, stackTopStr ye gore varsa gecerli state bulunur
vector<string> stateFinder(const string &currentState, const string &inputStr, const string &stackTopStr, vector<vector<string> > &partTVector){;
    // bulunan state in olacagi vector
    vector<string> returnVector;

    // butun partTVector de donerek uygun state var mi diye bakıyor
    for (int i = 0; i < partTVector.size(); ++i) {
        if ((partTVector[i][0] == currentState) && ((partTVector[i][1] == inputStr) || (partTVector[i][1] == "e")) && ((partTVector[i][2] == stackTopStr) || (partTVector[i][2] == "e"))){
            return partTVector[i];
        }
    }

    //uygun bir state bulamadigi icin icinde sadece currentState olan bi vector dondurur

    returnVector.push_back(currentState);
    return returnVector;
}

// stack in outputFile a uygun sekilde basilmasi
string stackPrinter(stack<string> stack){
    string stackStr = "";
    while (!stack.empty()){
        string str = stack.top();
        stack.pop();
        stackStr.insert(0,",");
        stackStr.insert(0,str);
    }

    stackStr.insert(0,"[STACK]:");

    if (stackStr.length() > 8){
        stackStr.pop_back();
    }

    return stackStr;
}

// icine gelen string i "," lerden ayirir
vector<string> split2(const string& text){
    vector<string> elemanlar;
    string strToken;

    stringstream ss(text);
    while(getline(ss, strToken, ',')){
        elemanlar.push_back(strToken);
    }
    return elemanlar;
}

// verilen string verilen list e de var mi diye kontrol eder
bool isInList(const string& item, const vector<string>& list){
    for (int i = 0; i < list.size(); ++i) {
        if (item == list[i]){
            return true;
        }
    }
    return false;
}