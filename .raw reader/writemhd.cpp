#include <iostream>
#include <fstream>
using namespace std;
void parseForNums(char* str, unsigned int* out)
{
   int i = 0;
   while(*str != NULL)
   {
      if(isdigit(*str))
      {
         out[i++] = (unsigned int)strtol(str, &str,10);
         //cout<<out[i-1] <<endl;
      }
      else
      {
         str++;
      }
   }
}

void writeMHD(char * file)
{
   ofstream myFile;
   std::string str(file);
   std::string filename = str.substr(0,str.find("."))+".raw";
   myFile.open(file);
   unsigned int* p = new unsigned int[4];
   parseForNums(file,p);
   myFile << "NDims = 3\n";
   myFile << "DimSize = ";
   for(int i = 0; i<3;i++)
   {
      myFile <<p[i] << " ";
   }

   myFile<<"\nElementType = MET_USHORT\nElementSpacing = 1 1 1\nBinaryDataByteOrderMSB = False\nElementDataFile = "<< filename.c_str()<<"\n";

   myFile.close();
}

int main(int argc, char* argv[]){
   if(argc <2)
   {
      cout<<"Usage: .exe [file.mhd]";
   }
   writeMHD(argv[1]);
}