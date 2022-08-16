#include <iostream>
#include <windows.h>
#include <string.h>
#include <Commdlg.h>
#include <vector>
#include <fstream>
#include<algorithm>
#include <iterator>



OPENFILENAME ofn ;

std::vector<std::string> multiFileParse(std::vector<std::string> files);
std::string outPath;
char szFile[10000];
char outFile[10000];
int mode;


using namespace std;

std::vector<std::string> files;

//Creates file explorer dialogue box and stores resultant file name inside vector
void FileInput()
{
    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL  ;
    ofn.lpstrFile = szFile ;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
    ofn.lpstrFilter = "All Files\0*.*\0\0";
    ofn.nFilterIndex =1;
    ofn.lpstrFileTitle = NULL ;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir=NULL ;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_EXPLORER;

    GetOpenFileName( &ofn );


    std::string path(ofn.lpstrFile);

    path += "\\";

    const char* current = ofn.lpstrFile + path.size();

    while(*current)
    {
        std::string file(current);
        files.push_back(path+file);
        current+=(file.size()+1);
    }
   
    if(files.size() == 0)
    {
        files.push_back(ofn.lpstrFile);
        multiFileParse(files);
    }

    else
    {
        multiFileParse(files);
    }
    

}
//control function for aggregate files, loops through file vector ignoring the first 9 lines since they have useless info
//removes extra string content, and then appends to output vector, removes duplicates and then writes to file
void aggregateMode(std::vector<std::string>)
{
    std::ifstream multiFile;
    std::string content;
    std::ofstream output;

    std::vector<std::string> fileContents;

    output.open(outPath, std::ios::app);
    for(int i = 0;i<files.size();i++)
    {
        multiFile.open(files[i]);
        std::cout<<files[i]<<std::endl;
        for(int i = 0;i<9;i++)
        {
            multiFile.ignore(10000,'\n');
        }

        while(std::getline(multiFile, content))
        {

            content = content.substr(0, content.find("OU=", 0));
            content.erase(0,3);
            content.erase(remove(content.begin(), content.end(), ','), content.end());
            fileContents.push_back(content);
            std::sort( begin(fileContents), end(fileContents) );
            fileContents.erase( std::unique( begin(fileContents), end(fileContents) ), end(fileContents) );


        }


        multiFile.close();



    }
    for(int j = 0;j<fileContents.size();j++)
    {
        output << fileContents[j] << std::endl;
    }
    output.close();
}
//removes extra useless strings just like aggr mode, then proceeds with group update logic:

//group logic essentially checks in the file already had a specific group name and compares with new incoming data
//if group name is a match, then find distance from group header to '' character which marks end of a group
//adds this group to a vector, and finds set difference between said vector and incoming content
//appends difference to a vector containing data that was already in the file, and writes that to file
//if a brand new group is added, then proceed with original functionality (just append to the file normally)
void groupMode(std::vector<std::string>)
{
    std::ifstream multiFile;
    std::string content;
    std::ofstream output;
    std::ifstream readOutput;
    std::string outputContent;
    std::vector<std::string> fileContents;
    std::set<std::string> outputSet;

    output.open(outPath, std::ios::app);

    readOutput.open(outPath);
    std::string firstLine;
    std::string outContent;
    std::vector<std::string> outCont;


    int lineCount = 0;
    std::vector<std::string> group;
    while(std::getline(readOutput, outContent))
    {

        outCont.push_back(outContent);
    }
    bool match = false;
    int distHeader;
    for(int i = 0;i<files.size();i++)
    {
        multiFile.open(files[i]);
        std::cout<<files[i]<<std::endl;
        if (std::getline(multiFile, firstLine))
        {
            firstLine.erase(0,10);
        }
        for(int i = 0;i<8;i++)
        {
            multiFile.ignore(10000,'\n');
        }
        fileContents.push_back(firstLine);
        while(std::getline(multiFile, content))
        {
            lineCount++;
            content = content.substr(0, content.find("OU=", 0));
            content.erase(0,3);
            content.erase(remove(content.begin(), content.end(), ','), content.end());
            fileContents.push_back(content);
        }
        fileContents.push_back("");
        multiFile.close();
        auto search = std::find(outCont.begin(), outCont.end(), fileContents[0]);

        if(search!=outCont.end()){
            match = true;
            distHeader = std::distance(outCont.begin(), search);

            auto foot = std::find(outCont.begin()+distHeader, outCont.end(), "");
            if(foot!=outCont.end()){

                int dist = std::distance(outCont.begin()+distHeader, foot);


                std::vector<std::string> group = std::vector<std::string>(outCont.begin() + distHeader+1, outCont.begin()+distHeader+dist);
                std::vector<std::string> symmDiff;
                std::sort(group.begin(), group.end());
                std::sort(fileContents.begin(), fileContents.end());

                std::set_difference(
                        fileContents.begin(), fileContents.end(),
                        group.begin(), group.end(),
                        std::back_inserter(symmDiff));


                outCont.insert(outCont.begin()+distHeader+1, symmDiff.begin(), symmDiff.end());



                std::cout<<*(outCont.begin()+distHeader+1)<<std::endl;


            }

        }
    }
    if(match)
    {
        std::ofstream update;
        update.open(outPath);

        for(int k = 0;k<outCont.size();k++)
        {
            update << outCont[k] << std::endl;
        }
        update.close();
        readOutput.close();
    }
    else
    {
        for(int j = 0;j<fileContents.size();j++)
        {
            output << fileContents[j] << std::endl;
        }
        output.close();
        readOutput.close();
    }
}

//select modes based on user input

std::vector<string> multiFileParse(std::vector<std::string> files)
{
    std::cout<<mode<<std::endl;
    std::cout<<files[0]<<std::endl;
    switch(mode)
    {
        case 1:
            aggregateMode(files);
            break;
            case 2:
                groupMode(files);
                break;
                default:
                    break;


    }



    return files;
}


//main function, asks user for input
int WINAPI WinMain( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow )
{
    std::cout << "WELCOME TO ADVIEWER COMPILER" << std::endl;
    std::cout << std::endl;
   

    std::cout << "Please enter an output file PATH (where you want results to be stored" << std::endl;



    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL  ;
    ofn.lpstrFile = outFile ;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
    ofn.lpstrFilter = "All Files\0*.*\0\0";
    ofn.nFilterIndex =1;
    ofn.lpstrFileTitle = NULL ;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir=NULL ;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER;

    GetOpenFileName( &ofn );

    std::string p(ofn.lpstrFile);


    std::cout<<p<<std::endl;

    std::cout << "Please select one of three modes: " << std::endl;
    std::cout << std::endl;
    std::cout << "Type 1 for Aggregate Mode or 2 for Group Mode followed by the Enter key" << std::endl;
    std::cin >> mode;
    outPath = p;
    std::cout<<mode<<std::endl;
    string s = FileInput();

    return 0;
}
