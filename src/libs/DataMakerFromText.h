//
// Created by 18141 on 2023/4/20.
//
#include <fstream>
#include "DataMaker.h"

#ifdef WIN32
#define INPUT_MAKE_EXE "\\std\\input.exe"
//#define system(path) system((std::string("cmd /k ") + std::string(path) + std::string("")).c_str());
#else
#define INPUT_MAKE_EXE "/std/input.exe"
#endif

// 输入模板
const std::string inputTemplate = R"delimiter(
// templante

#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<random>
#include <stdlib.h>
#define ll long long

using namespace std;

random_device rd;
mt19937 mt(rd());
void make(__int32 num);

int main(int argc, char *argv[]) {
    int index = atoi(argv[1]);
    make(index);
	return 0;
}
)delimiter";

//unable to test
class DataMakerFromSourceText :public DataMakerFromCppSourceFile{
public:

protected:
    std::string source;
    std::string inputMakeFilePath;
    std::string inputMakeFileEXE;
    std::string inputMakeSource;
public:
    const std::string &getInputMakeSource() const {
        return inputMakeSource;
    }

    void setInputMakeSource(const std::string &inputMakeSource) {
        DataMakerFromSourceText::inputMakeSource = inputTemplate + '\n' + inputMakeSource;
    }

protected:
    void autoFilePath(){
        DataMakerFromEXE::defaultPathSet();
        inputMakeFilePath = DEFAULT_PATH + "/std/inputMaker.cpp";
    }
    void compileCppFiles(std::string cmd1, std::string path = DEFAULT_PATH + INPUT_MAKE_EXE){
        std::string Command = (cmd1 + " -o " + "\"" + path + "\"");
        int code = system(Command.c_str());
        std::clog << "Compile Command: " << Command << std::endl;
        std::clog << "Compile Code: " << code << std::endl;
        if(code){
            std::clog << &"Input Source Compile Error, Error code = " [ code] << std::endl;
            throw std::runtime_error(" Input Source Compilation Error");
        }
        inputMakeFileEXE = "" + DEFAULT_RUN_PATH + INPUT_MAKE_EXE + " ";
    }

    void make(int __test_num) override {
        DataMakerFromCppSourceFile::make(__test_num);
    }

    void makeInFile(std::string __INPUT_FILE__, int __test_num) override {
//        DataMaker::makeInFile(__INPUT_FILE__, __test_num);
        // 重写makeInFile函数，重新实现输入数据构造的方法
        std::clog << __INPUT_FILE__ << " :make begin" << std::endl;
        std::string Command = inputMakeFileEXE + std::to_string(__test_num) + " > " + __INPUT_FILE__;
        std::clog << "Run Command: " << Command << std::endl;
        int code = system(Command.c_str());
        if(code){
            std::clog << __INPUT_FILE__ << " make error : error code " + std::to_string(code) << __INPUT_FILE__ << std::endl;
            if(!ignoreMakeInputDataError){
                throw std::runtime_error("InputFileMakeError:make input file error!");
            }
            return;
        }
        std::clog << __INPUT_FILE__ << " :make done" << std::endl;
    }

    void setFiles(){
        std::ofstream inputMakerOut;
        inputMakerOut.open(inputMakeFilePath);
        inputMakerOut << inputMakeSource << std::endl;
        inputMakerOut.close();
        std::ofstream stdSourceOut;
        stdSourceOut.open(cppSourcePath);
        stdSourceOut << source << std::endl;
        stdSourceOut.close();
    }
public:
    void run() override {
        setFiles();
        std::string compileCmd;
        if (gccCompilePath != "") {
            compileCmd = gccCompilePath;
        }
        else {
            compileCmd = vcCompile;
        }
        std::clog << "Use C++ Compile: " << compileCmd << " ,C++ Version : C++ " << cppVersion << std::endl;
        compileCmd += " \"" + inputMakeFilePath + "\" -std=c++" + std::to_string(cppVersion);
        compileCppFiles( compileCmd);
        compileCppFile(cppSourcePath);
        std::clog << std::endl;
        std::clog << std::endl;
        if (inputMakeSource == "")throw std::runtime_error("NullPtrError:You are not set make function!");
        if (testcasePath == "")throw std::runtime_error("NoPathError:You are not set path!");
        if (testNum < 1)throw std::runtime_error("NoTestNumError:You are not set testnum!");
        if (cmd == "")throw std::runtime_error("NoStdSourceError:You are not set std Source!");
        for (int i = 1; i <= testNum; i++) {
            this->make(i);
        }
        std::clog << "All TestCases make done!" << std::endl;
    }
    const std::string &getSource() const {
        return source;
    }

    void setSource(const std::string &source) {
        DataMakerFromSourceText::source = source;
    }

    DataMakerFromSourceText() : DataMakerFromCppSourceFile() {
        autoFilePath();
    }

    explicit DataMakerFromSourceText(const std::string &source) : source(source) {
        autoFilePath();
    }
};