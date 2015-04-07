//
//  merlos.cpp
//  MERLOS
//
//  Created by Garret Lam on 18/7/14.
//  Copyright (c) 2014 Garret. All rights reserved.
//

#include "merlos.h"

void fileExist(fstream &file, string filename)
{
  if (!file) {
    cerr << "\n" << filename << " can't be opened!";
    cerr << "\nPlease check " << filename << " again!" << endl;
    exit(1);
  }
  return;
}

int main()
{
  string globalInfofilename = "GlobalToLocalMap.dat", wholelinedata, file = "Result-T", filetemp = ".dat", globalResultfilename = "Result-T", filelocal = "-LM", temp, temp2, dummy;
  int startNumber = -1, endNumber = -1, interval = 0, numberofProcesses = 0, rank = 0, GlobalID = 0, ID = 0, lineLength = 0, esize = 0, offset = 8, minRank = 1;
  vector<int> process, localID;
  char ftemp[8], f_rank[3];
  ifstream *localResultFile;
  ofstream globalResultFile;
  
  fstream globalInfo(globalInfofilename.c_str(), ios::in);
  fileExist(globalInfo,globalInfofilename.c_str());
  
  cout << "\nMERLOS - The merging of local solutions to the global solutions for CE/SE MPI program";
  cout << "\nVersion 1.0";
  cout << "\nCopyright (c) 2014 Garret. All rights reserved.";
  cout << "\n";
  cout << "\nPlease enter the starting number of file name for combining: ";
  cin >> startNumber;
  cout << "\nPlease enter the ending number of file name for combining: ";
  cin >> endNumber;
  cout << "\nPlease enter the interval of file name for combining: ";
  cin >> interval;
  cout << "\nPlease enter the prefix of the file: ";
  cin >> globalResultfilename;
  
  // Read global mesh to local mesh information (m, localMesh[m][k], k+1)
  temp2 = globalResultfilename;
  file = globalResultfilename;
  cout << setiosflags(ios::left) << setw(70) << setfill('.') << "\nReading global to local map";
  while (globalInfo >> rank) { // vector container
    globalInfo >> ID;
    globalInfo >> GlobalID;
    process.push_back(rank);
    localID.push_back(ID);
    if (rank > numberofProcesses)
      numberofProcesses = rank;
    if (rank < minRank)
      minRank = rank;
  }

  numberofProcesses+= 1 - minRank; // if rank start from 0
  cout << "Done!";
  
  // Open local result files
  for (int m = startNumber; m < endNumber; m+=interval) {
    sprintf(ftemp,"%8.8d",m);
    file.append(ftemp);
    file.append(filelocal);
    temp = file;
    globalResultfilename.append(ftemp);
    globalResultfilename.append(filetemp);
    cout << "\nProcessing " << setiosflags(ios::left) << setw(58) << setfill('.') << globalResultfilename;
    
    // Open local result file
    if (m == startNumber)
      localResultFile = new ifstream[numberofProcesses];
    for (int r = 0; r < numberofProcesses; r++) {
      sprintf(f_rank,"%3.3d",r);
      file.append(f_rank);
      file.append(filetemp);
      localResultFile[r].open(file.c_str(), ifstream::in);
      file = temp;
    }
    
    // Load local result file for global ID
    esize = process.size();
    globalResultFile.open(globalResultfilename.c_str(), std::ofstream::out);
    for (int k = 0; k < esize; k++) {
      // Check the length of 1 line
      if (k == 0) {
        getline(localResultFile[0], wholelinedata);
        lineLength = localResultFile[0].tellg();
      }
      // Read local file
      localResultFile[process[k]].seekg((localID[k]-1) * lineLength + offset, localResultFile[process[k]].beg);
      getline(localResultFile[process[k]], wholelinedata);
      // Write global file
      globalResultFile << setiosflags(ios::right) << setw(8) << k+1 << wholelinedata << "\n";
    }
    
    // Close all files and reset variables
    file = temp2;
    globalResultfilename = temp2;
    globalResultFile.close();
    for (int r = 0; r < numberofProcesses; r++)
      localResultFile[r].close();
    cout << "Done!\n";
  }
  
  return 0;
}
