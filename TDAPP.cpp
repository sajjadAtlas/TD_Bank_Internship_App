#include <iostream>

#include <string.h>
#include <vector>
#include <fstream>
#include<algorithm>
#include <iterator>

#include <set>

void multiFileParse(int mode);

std::string outPath;

int mode;
std::string inputFile;
std::string outputFile;

std::vector<std::string> files;

/*
Function for selecting between Aggregate and Group modes
*/

int selectMode()
{
    
    std::cout << "Please type the number 1 for Aggregate Mode or the number 2 for Group mode followed by the Enter key"<<std::endl;

    std::cin >> mode;

    std::cout<< "Please enter an input file path (file you want to parse)" <<std::endl;

    std::cin >> inputFile;
    
    std::cout << "Please enter an output file PATH (where you want results to be stored)" << std::endl;
    
    std::cin >>outputFile;
    
    return mode;
}

/*
This function compiles an aggregate list of all the names from N files, and automatically removes duplicates.
Removing duplicates is achieving by reading previous state of csv file into a set, and then inserting new content to ensure
only unique values are added 
*/

void aggregateMode()
{
    std::ifstream multiFile;
    std::string content;
    std::string cont;
    std::ofstream output;
    std::ifstream readOutput;
    std::vector<std::string> fileContents;
    std::set<std::string> fileConts;

    readOutput.open(outputFile);

        while(std::getline(readOutput, cont))
        {
            fileConts.insert(cont);

        }

        readOutput.close();

        multiFile.open(inputFile);
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

        }
        for(int i = 0;i<fileContents.size();i++)
        {
            fileConts.insert(fileContents[i]);
        }

        multiFile.close();
        readOutput.close();

        output.open(outputFile);

    for(auto elem: fileConts)
    {
        output << elem << std::endl;
    }
    output.close();
}

/* 
Group mode sorts names in a list by which group they belong to.
To do this, we read the previous state of the file and see if the first line of the incoming data (which is set to the group name) matches
data inside the previous state. Then, upon matching, saving items from group header to end of group, which is denoted by a blank line into a vector.
Then compare this vector to the incoming data vector and save the difference between them, which will be appended to the previous state, 
thereby updating groups. If there is no match between group headers, simply insert the input data vector into the file normally, with group header as item 0.
*/

void groupMode()
{
    std::ifstream multiFile;
    std::string content;
    std::ofstream output;
    std::ifstream readOutput;
    std::string outputContent;
    std::vector<std::string> fileContents;

    output.open(outputFile, std::ios::app);

    readOutput.open(outputFile);
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

    multiFile.open(inputFile);
    
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


            std::vector<std::string> group = std::vector<std::string>(outCont.begin() + distHeader, outCont.begin()+distHeader+dist);
            std::vector<std::string> symmDiff;
            std::sort(group.begin(), group.end());
            std::sort(fileContents.begin(), fileContents.end());

            std::set_difference(
                    fileContents.begin(), fileContents.end(),
                    group.begin(), group.end(),
                    std::back_inserter(symmDiff));


            outCont.insert(outCont.begin()+distHeader+1, symmDiff.begin(), symmDiff.end());

            outCont.erase(outCont.begin()+distHeader+1);
        }

    }

    if(match)
    {
        std::ofstream update;
        update.open(outputFile);

        for(int k = 0;k<outCont.size();k++)
        {
            update << outCont[k] << std::endl;
        }
        update << " " << std::endl;
        update.close();
        readOutput.close();
    }
    else
    {
        for(int j = 0;j<fileContents.size();j++)
        {
            output << fileContents[j] << std::endl;
        }
        output << " " << std::endl;
        output.close();
        readOutput.close();
    }

}

//Calls appropriate functions based on user mode selection

void multiFileParse(int mode)
{
    switch(mode)
    {
        case 1:
            aggregateMode();
            break;
        case 2:
            groupMode();
            break;
        default:
            break;

    }

}

int main()
{
    std::cout << "WELCOME TO ADVIEWER COMPILER" << std::endl;
    std::cout << std::endl;
    std::cout << "Before you start, please make sure you have read the help guide!" << std::endl;
    std::cout << std::endl;
    int stateSelect = 1;


   while(stateSelect)
   {

        int selection = selectMode();
        multiFileParse(selection);

        std::cout<<"Press 0 to quit, or press 1 to parse more files, followed by the Enter key"<<std::endl;
        std::cin >> stateSelect;
        

   }

    return 0;
}
