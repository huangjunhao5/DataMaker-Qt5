//
// Created by 18141 on 2023/4/20.
//

#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#ifdef _WIN32
#include <direct.h>
#elif __APPLE__ || __linux__
#include<unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#endif

#define MAX_PATH_LEN 256

#ifdef WIN32
#define ACCESS(fileName,accessMode) _access(fileName,accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName,accessMode) access(fileName,accessMode)
#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include <sys/types.h>
#include <pwd.h>
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <filesystem>

std::string getNowRunPath(){

    std::string res = "";
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        homeDir = getpwuid(getuid())->pw_dir;
    }

    // Get the documents directory.
    std::string documentsPath = homeDir;
    documentsPath += "/Documents";
    res = documentsPath;
    res += "/testData";
    MKDIR(res.c_str());
    std::error_code error;
    std::filesystem::permissions(res, std::filesystem::perms::owner_write, std::filesystem::perm_options::add, error);

    if (error) {
        std::cerr << "Error setting permissions: " << error.message() << std::endl;
        throw std::runtime_error("Error setting permissions: " + error.message());
    }


    return res;
}
#else
std::string getNowRunPath(){
    char runPath[1024] = {0};
    getcwd(runPath, sizeof(runPath));
    return std::string(runPath);
}
#endif


// 抽象类DataMaker：未实现make函数和run函数，需要重写make函数和run函数
// make函数编写说明：传入一个参数，为当前制造的数据编号（第几组数据）
// 调用数据路径生成函数生成测试数据的全路径（包括.in文件和.out文件，1为in，0为out）
// 然后使用freopen打开测试数据文件
// 再调用传入的fun函数生成数据
// fun函数可以使用cout构造数据
// run函数需要进行基本的运行判断，判断生成数据的必要参数是否具备
// 否则抛出异常
// 如果使用无参构造函数，将使用默认的生成参数设置
// 数据生成的路径：程序目录下的/data/文件夹下
// 数据的组数：12
// 默认的数据生成函数不提供，如果没有传入数据生成函数，将抛出异常

std::string DEFAULT_PATH = "";
std::string DEFAULT_RUN_PATH = "";
const int DEFAULT_TEST_CASE = 12;

class DataMaker
{
protected:
    typedef void (*MakeTestFun)(int testcase);
    std::string testcasePath;
public:
    const std::string &getTestcasePath() const {
        return testcasePath;
    }

    void setTestcasePath(const std::string &testcasePath) {
        DataMaker::testcasePath = testcasePath;
    }

protected:

    MakeTestFun testMakeFunction = nullptr;
    int testNum = 12;

    bool ignoreMakeInputDataError = 0;

    bool ignoreMakeOutputDataError = 0;

    // 构造所需文件的全路径，如果未指定路径，抛出异常
    std::string fileNameMaker(int name, std::string testPath, bool __IS_INPUT_FILE) {
        if (testPath == "")throw std::runtime_error("NoFilePathError:You are not set file path!");
        char ts[1000];
        int tf = name;
        int len = 0;
        memset(ts, 0, sizeof(ts));
        while (tf) {
            ts[++len] = tf % 10 + '0';
            tf /= 10;
        }
        char s[1000];
        memset(s, 0, sizeof(s));
        for (int i = 1; i <= strlen(ts + 1); i++) {
            s[len - i] = ts[i];
        }
        std::string fname = ".in";
        if (!__IS_INPUT_FILE)fname = ".out";
        fname = s + fname;
        return "\"" + testPath + fname + "\"";
    }

    //使用fun函数构造数据
    virtual void makeInFile(std::string __INPUT_FILE__ ,int __test_num) {
//        __INPUT_FILE__ = "\"" + __INPUT_FILE__ + "\"";
        std::clog << __INPUT_FILE__ << " :make begin" << std::endl;
        auto it = freopen(__INPUT_FILE__.c_str(), "w", stdout);
        if (it == nullptr) {
            //输入文件打开失败，抛出异常
            std::clog << __INPUT_FILE__ << " make error : can't open file path" << __INPUT_FILE__ << std::endl;
            if(!ignoreMakeInputDataError){
                throw std::runtime_error("InputFileMakeError:make input file error!");
            }
            return;
        }
        {
            //在这里构造数据
            testMakeFunction(__test_num);
        }
        fclose(stdout);
        std::clog << __INPUT_FILE__ << " :make done" << std::endl;
    }

    virtual void make(int __test_num) = 0;
public:
    virtual void run() = 0;
    void setDEFAULT_RUN_PATH(){
        int len = DEFAULT_RUN_PATH.length();
        bool flag = 0;
        for(int i = 0;i < len;i++){
            if(DEFAULT_RUN_PATH[i] =='\\' || DEFAULT_RUN_PATH[i] == '/'){
                if(flag){
                    DEFAULT_RUN_PATH.insert(i, "\"");
                    i++;
                    len++;
                }else{
                    flag = true;
                }
                DEFAULT_RUN_PATH.insert(i + 1, "\"");
                i++;
                len++;
            }
        }
        DEFAULT_RUN_PATH.push_back('"');
    }
    void getNowPath(){
        DEFAULT_PATH = getNowRunPath();
        DEFAULT_RUN_PATH = DEFAULT_PATH;
        setDEFAULT_RUN_PATH();
//        DEFAULT_PATH += "/data/";
        MKDIR((DEFAULT_RUN_PATH + "/data/").c_str());
    }
    virtual void defaultPathSet() {
        getNowPath();
        testcasePath = DEFAULT_PATH + "/data/";
        testNum = DEFAULT_TEST_CASE;
        MKDIR(testcasePath.c_str());
    }
    DataMaker() { defaultPathSet(); }
    DataMaker(MakeTestFun makeTestFun) :testMakeFunction(makeTestFun) {defaultPathSet();}
    //构造函数：提供无参构造函数，有参构造函数有3个参数,参数位置随意
    //可以省略测试数据个数，默认为12组数据
    DataMaker(std::string path, MakeTestFun func, int testNum = 12) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}
    DataMaker(std::string path, int testNum, MakeTestFun func) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}
    DataMaker(int testNum, std::string path, MakeTestFun func) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}
    DataMaker(int testNum, MakeTestFun func, std::string path) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}
    DataMaker(MakeTestFun func, int testNum, std::string path) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}
    DataMaker(MakeTestFun func, std::string path, int testNum = 12) : testcasePath(path), testMakeFunction(func), testNum(testNum) {getNowPath();}

    void setPath(std::string path) {
        this->testcasePath = path;
    }
    void setTestNum(int num) {
        this->testNum = num;
    }
    void setMakeTestFunc(MakeTestFun func) {
        this->testMakeFunction = func;
    }

    void setignoreMakeInputDataError(bool model) {
        this->ignoreMakeInputDataError = model;
    }
    void setignoreMakeOutputDataError(bool model) {
        this->ignoreMakeOutputDataError = model;
    }
};


// SpecialJudgeDataMaker：继承自DataMaker类，用于实现生成SpecialJudge题目的数据
// 此类生成数据时，必须要传入数据生成方法
// 生成的数据默认存放在程序目录下的/data/文件夹下
// 如果不忽略数据生成异常，默认在数据生成错误时抛出异常中断程序
// 如果使用无参构造函数，将使用默认配置
class SpecialJudgeDataMaker :public DataMaker {
protected:
    virtual void make(int __test_num) override {
        std::clog << "Test" << __test_num << " :make begin" << std::endl;
        std::string __INPUT_FILE__ = fileNameMaker(__test_num, testcasePath, 1);

        makeInFile(__INPUT_FILE__, __test_num);
        //std解法

        //Special Judge不产生.out文件，将跳过输出数据生成
        //使用std标程重定向到文件输出.out文件

        std::clog << "Special Judge Test , Skip OutputFile Create !" << std::endl;

        std::clog << "Test" << __test_num << " :make done!" << std::endl;
        std::clog << std::endl;
    }
public:
    virtual void run() override {
        if (testMakeFunction == nullptr)throw std::runtime_error("NullPtrError:You are not set make function!");
        if (testcasePath == "")throw std::runtime_error("NoPathError:You are not set path!");
        if (testNum < 1)throw std::runtime_error("NoTestNumError:You are not set testnum!");
        for (int i = 1; i <= testNum; i++) {
            make(i);
        }
        std::clog << "All TestCases make done!" << std::endl;
    }

    virtual void defaultPathSet() override {
        DataMaker::defaultPathSet();
    }

    SpecialJudgeDataMaker() { this->defaultPathSet(); }
    //继承父类构造函数，不提供新的构造函数（不需要新构造函数）
    //构造函数：提供无参构造函数，有参构造函数有3个参数,参数位置随意
    //可以省略测试数据个数，默认为12组数据
    SpecialJudgeDataMaker(std::string path, MakeTestFun func, int testNum = 12) : DataMaker(path, func, testNum) {getNowPath();}
    SpecialJudgeDataMaker(std::string path, int testNum, MakeTestFun func) : DataMaker(path, func, testNum) {getNowPath();}
    SpecialJudgeDataMaker(int testNum, std::string path, MakeTestFun func) : DataMaker(path, func, testNum) {getNowPath();}
    SpecialJudgeDataMaker(int testNum, MakeTestFun func, std::string path) : DataMaker(path, func, testNum) {getNowPath();}
    SpecialJudgeDataMaker(MakeTestFun func, int testNum, std::string path) : DataMaker(path, func, testNum) {getNowPath();}
    SpecialJudgeDataMaker(MakeTestFun func, std::string path, int testNum = 12) : DataMaker(path, func, testNum) {getNowPath();}
};


// DataMakerFromEXE：继承自DataMaker类，用于实现生成数据，使用std可执行文件生成数据
// 此类生成数据时，必须要传入数据生成方法，默认使用程序所在路径下的/std/std.cpp生成数据
// 生成的数据默认存放在程序目录下的/data/文件夹下
// 如果不忽略数据生成异常，默认在数据生成错误时抛出异常中断程序
class DataMakerFromEXE :public DataMaker {
protected:
    std::string cmd;

    //使用标程的EXE文件构造数据
    virtual void makeOutFileEXE(std::string __INPUT_FILE__, std::string __OUTPUT_FILE__, int __test_num) {
        std::clog << __OUTPUT_FILE__ << " :make begin" << std::endl;
        auto tcmd = cmd;
        //cmd字符串为std程序的全路径
        tcmd += " < " + __INPUT_FILE__;
        tcmd += " > " + __OUTPUT_FILE__;
        int returnNum = system(tcmd.c_str());
        if (returnNum) {
            //调用标程时出现错误，抛出异常
            std::clog << __OUTPUT_FILE__ << " :make failed , error num = " << returnNum << std::endl;
            if (!ignoreMakeOutputDataError) {
                throw std::runtime_error("OutputFileMakeError:make output file error!");
            }
        }
        else {
            std::clog << __OUTPUT_FILE__ << " :make done" << std::endl;
        }
    }

    virtual void make(int __test_num) override {
        std::clog << "Test" << __test_num << " :make begin" << std::endl;
        std::string __INPUT_FILE__ = fileNameMaker(__test_num, testcasePath, 1);
        std::string __OUTPUT_FILE__ = fileNameMaker(__test_num, testcasePath, 0);

        makeInFile(__INPUT_FILE__, __test_num);
        //std解法

        //Special Judge不产生.out文件，将跳过输出数据生成
        //如果不是SpecialJudge，生成.out文件
        makeOutFileEXE(__INPUT_FILE__, __OUTPUT_FILE__, __test_num);

        //使用std标程重定向到文件输出.out文件

        std::clog << "Test" << __test_num << " :make done!" << std::endl;
        std::clog << std::endl;
    }

public:
    virtual void run() override {
        if (testMakeFunction == nullptr)throw std::runtime_error("NullPtrError:You are not set make function!");
        if (testcasePath == "")throw std::runtime_error("NoPathError:You are not set path!");
        if (testNum < 1)throw std::runtime_error("NoTestNumError:You are not set testnum!");
        if (cmd == "")throw std::runtime_error("NoStdSourceError:You are not set std Source!");
        for (int i = 1; i <= testNum; i++) {
            make(i);
        }
        std::clog << "All TestCases make done!" << std::endl;
    }

    virtual void defaultPathSet() override {
        DataMaker::defaultPathSet();
        MKDIR((DEFAULT_PATH + "/std/").c_str());
        cmd = DEFAULT_RUN_PATH + "/std/a.exe";
    }

    void setstdEXEPath(std::string stdEXEPath) {
        this->cmd = stdEXEPath;
    }

    DataMakerFromEXE() { this->defaultPathSet(); }
    DataMakerFromEXE(MakeTestFun makeTestFun): DataMaker(makeTestFun){this->defaultPathSet();}
    //构造函数：提供无参构造函数，有参构造函数有4个参数，第一个参数必须为std程序的路径，其余参数随意
    //可以省略测试数据个数，默认为12组数据
    DataMakerFromEXE(std::string stdEXEPath, std::string path, MakeTestFun func, int testNum = 12) : DataMaker(path,func,testNum) {
        setstdEXEPath(stdEXEPath);
    }
    DataMakerFromEXE(std::string stdEXEPath, std::string path, int testNum, MakeTestFun func) : DataMaker(path, func, testNum) {
        setstdEXEPath(stdEXEPath);
    }
    DataMakerFromEXE(std::string stdEXEPath, int testNum, std::string path, MakeTestFun func) : DataMaker(path, func, testNum) {
        setstdEXEPath(stdEXEPath);
    }
    DataMakerFromEXE(std::string stdEXEPath, int testNum, MakeTestFun func, std::string path) : DataMaker(path, func, testNum) {
        setstdEXEPath(stdEXEPath);
    }
    DataMakerFromEXE(std::string stdEXEPath, MakeTestFun func, int testNum, std::string path) : DataMaker(path, func, testNum) {
        setstdEXEPath(stdEXEPath);
    }
    DataMakerFromEXE(std::string stdEXEPath, MakeTestFun func, std::string path, int testNum = 12) : DataMaker(path, func, testNum) {
        setstdEXEPath(stdEXEPath);
    }

    //继承父类构造函数
    //构造函数：提供无参构造函数，有参构造函数有3个参数,参数位置随意
    //可以省略测试数据个数，默认为12组数据
    DataMakerFromEXE(std::string path, MakeTestFun func, int testNum = 12) : DataMaker(path,func,testNum) {}
    DataMakerFromEXE(std::string path, int testNum, MakeTestFun func) : DataMaker(path, func, testNum) {}
    DataMakerFromEXE(int testNum, std::string path, MakeTestFun func) : DataMaker(path, func, testNum) {}
    DataMakerFromEXE(int testNum, MakeTestFun func, std::string path) : DataMaker(path, func, testNum) {}
    DataMakerFromEXE(MakeTestFun func, int testNum, std::string path) : DataMaker(path, func, testNum) {}
    DataMakerFromEXE(MakeTestFun func, std::string path, int testNum = 12) : DataMaker(path, func, testNum) {}
};


// DataMakerFromCppSourceFile: 继承自DataMakerFromEXE类，用于加载C++源文件，使用std源文件生成数据
// 此类生成数据时，必须传入数据生成的方法，默认使用程序所在路径下的/std/std.cpp生成数据
// 生成的数据默认存放在程序目录下的/data/文件夹下
class DataMakerFromCppSourceFile :public DataMakerFromEXE {
protected:
    // cmd为c++源文件路径

    std::string gccCompilePath;

    std::string vcCompile;

    std::string cppSourcePath;

    int cppVersion = 17;
public:
    int getCppVersion() const {
        return cppVersion;
    }

    void setCppVersion(int cppVersion) {
        DataMakerFromCppSourceFile::cppVersion = cppVersion;
    }
    const std::string &getGccCompilePath() const {
        return gccCompilePath;
    }

    void setGccCompilePath(const std::string &gccCompilePath) {
        DataMakerFromCppSourceFile::gccCompilePath = gccCompilePath;
    }

    const std::string &getVcCompile() const {
        return vcCompile;
    }

    void setVcCompile(const std::string &vcCompile) {
        DataMakerFromCppSourceFile::vcCompile = vcCompile;
    }

    const std::string &getCppSourcePath() const {
        return cppSourcePath;
    }

    void setCppSourcePath(const std::string &cppSourcePath) {
        DataMakerFromCppSourceFile::cppSourcePath = "" + cppSourcePath + "";
    }

protected:

    virtual void autoCompilePath() {
        int gccReturnNum = system("g++ -v");
        int vcReturnNum = system("cl.exe -v");
        if (!gccReturnNum) {
            gccCompilePath = "g++";
        }
        if (!vcReturnNum) {
            vcCompile = "cl.exe";
        }
    }

    void compileCppFile(std::string stdSourceFilePath) {
        if (gccCompilePath == "" && vcCompile == "") {
            std::clog << "compile path not set,system will auto get comple path" << std::endl;
            autoCompilePath();
        }
        if (gccCompilePath == "" && vcCompile == "") {
            std::clog << "std Source Compilation error : system can't get Compile path" << std::endl;
            throw std::runtime_error(" NoCompilePathError:system path has no Compile path");
        }
        std::string compileCmd;
        if (gccCompilePath != "") {
            compileCmd = gccCompilePath;
        }
        else {
            compileCmd = vcCompile;
        }
        std::clog << "Use C++ Compile: " << compileCmd << " ,C++ Version : C++ " << cppVersion << std::endl;
        compileCmd += " \"" + stdSourceFilePath + "\" -std=c++" + std::to_string(cppVersion);
        compileCpp(compileCmd);
    }
    // 编译C++代码得到std程序
    virtual void compileCpp(std::string cmd, std::string outPath = "/std/a.exe"){
        std::string path = DEFAULT_PATH + outPath;
        // 调用g++，生成可执行文件
        int code = system((cmd + " -o " + "\"" + path + "\"").c_str());
        std::clog << "Compile Command: " << cmd + " -o " + "\"" + path + "\"" << std::endl;
        std::clog << "Compile Code: " << code << std::endl;

        if(code){
            // 编译错误
            std::cerr << &"Compilation Error, Error code = " [ code] << std::endl;
            throw std::runtime_error(" std Source Compilation Error");
        }
        DataMakerFromEXE::setstdEXEPath(DEFAULT_RUN_PATH + outPath);
    }

    virtual void make(int __test_num) override {
        DataMakerFromEXE::make(__test_num);
    }

    void loadDefault(){
        setCppSourcePath(DEFAULT_PATH + "/std/std.cpp");
        std::clog << "Use default std C++ Source :" + DEFAULT_PATH + "/std/std.cpp" <<std::endl;
    }
public:
    virtual void run() override{
        compileCppFile(cppSourcePath);
        std::clog << std::endl;
        std::clog << std::endl;
        DataMakerFromEXE::run();
    }
    DataMakerFromCppSourceFile(): DataMakerFromEXE(){
        autoCompilePath();
        loadDefault();
    }

    DataMakerFromCppSourceFile(MakeTestFun makeTestFun): DataMakerFromEXE(makeTestFun){
        autoCompilePath();
        loadDefault();
    }

    DataMakerFromCppSourceFile(std::string cppSource): DataMakerFromEXE(){
        setCppSourcePath(cppSource);
        std::clog<< "path:" + cppSourcePath << std::endl;
        autoCompilePath();
    }
    //继承父类构造函数
    DataMakerFromCppSourceFile(std::string path, MakeTestFun func, int testNum = 12) : DataMakerFromEXE(path,func,testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string path, int testNum, MakeTestFun func) : DataMakerFromEXE(path, func, testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(int testNum, std::string path, MakeTestFun func) : DataMakerFromEXE(path, func, testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(int testNum, MakeTestFun func, std::string path) : DataMakerFromEXE(path, func, testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(MakeTestFun func, int testNum, std::string path) : DataMakerFromEXE(path, func, testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(MakeTestFun func, std::string path, int testNum = 12) : DataMakerFromEXE(path, func, testNum) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource, std::string path, MakeTestFun func, int testNum = 12) : DataMakerFromEXE(path,func,testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource,std::string path, int testNum, MakeTestFun func) : DataMakerFromEXE(path, func, testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource,int testNum, std::string path, MakeTestFun func) : DataMakerFromEXE(path, func, testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource,int testNum, MakeTestFun func, std::string path) : DataMakerFromEXE(path, func, testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource,MakeTestFun func, int testNum, std::string path) : DataMakerFromEXE(path, func, testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
    DataMakerFromCppSourceFile(std::string stdCppSource,MakeTestFun func, std::string path, int testNum = 12) : DataMakerFromEXE(path, func, testNum),cppSourcePath(stdCppSource) {
        autoCompilePath();
    }
};

// unable to test
class JudgeSubmit :public DataMakerFromCppSourceFile{};

