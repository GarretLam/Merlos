//
//  merlos.cpp
//  MERLOS
//
//  Created by Garret Lam on 18/7/14.
//  Copyright (c) 2016 Garret. All rights reserved.
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
  string globalInfofilename = "GlobalToLocalMap.dat", wholelinedata, file = "Result-T", filetemp = ".dat", filetemp2 = ".bin", globalResultfilename = "Result-T", filelocal = "-LM", temp, temp2, dummy, str;
  int startNumber = -1, endNumber = -1, interval = 0, numberofProcesses = 0, rank = 0, GlobalID = 0, ID = 0, lineLength = 0, esize = 0, offset = 8, minRank = 1, inBinary = 0, outBinary = 0, data1 = -1, dimensionality = 0, numVar = 0, mergingType = 0, offsetInt = 0, Step = 0, dummyInt = 0;
  vector<int> process, localID;
  char ftemp[8], f_rank[3];
  char* buffer;
  ifstream *localResultFile;
  ofstream globalResultFile;
  double data[16] = {}, Time = 0.0, timeInc = 0.0, nexttime = 0.0;

  fstream globalInfo(globalInfofilename.c_str(), ios::in);
  fileExist(globalInfo,globalInfofilename.c_str());

  cout << "\nMERLOS - The merging of local solutions to the global solutions for CE/SE MPI program";
  cout << "\nVersion 1.2";
  cout << "\nCopyright (c) 2016 Garret. All rights reserved.";
  cout << "\n";
  cout << "\nPlease enter the starting number of file name for combining: ";
  cin >> startNumber;
  cout << "\nPlease enter the ending number of file name for combining: ";
  cin >> endNumber;
  cout << "\nPlease enter the interval of file name for combining: ";
  cin >> interval;
  cout << "\nPlease enter the prefix of the file: ";
  cin >> globalResultfilename;
  cout << "\nPlease enter the dimensionality of the file (2: 2D  3:3D): ";
  cin >> dimensionality;
  cout << "\nPlease enter the data type of the input files (0: ASCII  1: Binary):";
  cin >> inBinary;
  cout << "\nPlease enter the data type of the output files (0: ASCII  1: Binary):";
  cin >> outBinary;
  cout << "\nPlease enter the format of the input files (0: Results  1: Intermediate solutions):";
  cin >> mergingType;

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

  if (dimensionality == 2) {
    if (mergingType) {
      numVar = 12;
    } else
      numVar = 9;
  } else {
    if (mergingType) {
      numVar = 15;
    } else
    numVar = 16;
  }

  // Open local result files
  switch (inBinary) {
    case 1: // Binary input
      for (int m = startNumber; m < endNumber; m+=interval) {
        sprintf(ftemp,"%8.8d",m);
        file.append(ftemp);
        if (mergingType)
          filelocal = "LM";
        file.append(filelocal);
        temp = file;
        globalResultfilename.append(ftemp);
        if (outBinary == 1)
          globalResultfilename.append(filetemp2);
        else
          globalResultfilename.append(filetemp);
        cout << "\nProcessing " << setiosflags(ios::left) << setw(58) << setfill('.') << globalResultfilename;

        // Open local result file
        if (m == startNumber)
          localResultFile = new ifstream[numberofProcesses];
        for (int r = 0; r < numberofProcesses; r++) {
          sprintf(f_rank,"%3.3d",r);
          file.append(f_rank);
          if (inBinary == 1)
            file.append(filetemp2);
          else
            file.append(filetemp);
          localResultFile[r].open(file.c_str(), ios::in | ios::binary);
          file = temp;
        }

        // Load local result file for global ID
        esize = process.size();
        if (outBinary == 1)
          globalResultFile.open(globalResultfilename.c_str(), std::ofstream::out | ios::binary);
        else
          globalResultFile.open(globalResultfilename.c_str(), std::ofstream::out);

        if (mergingType) {
          localResultFile[0].read(reinterpret_cast<char *>(&Time), sizeof(double));
          localResultFile[0].read(reinterpret_cast<char *>(&timeInc), sizeof(double));
          localResultFile[0].read(reinterpret_cast<char *>(&Step), sizeof(int));
          localResultFile[0].read(reinterpret_cast<char *>(&nexttime), sizeof(double));
          offsetInt = 8*3+4;
          if (outBinary == 1) {
            globalResultFile.write(reinterpret_cast<const char *>(&Time), sizeof(double));
            globalResultFile.write(reinterpret_cast<const char *>(&timeInc), sizeof(double));
            globalResultFile.write(reinterpret_cast<const char *>(&Step), sizeof(int));
            globalResultFile.write(reinterpret_cast<const char *>(&nexttime), sizeof(double));
          } else {
            globalResultFile << setiosflags(ios::right) << setiosflags (ios::fixed | ios::showpoint) << setw(24) << setprecision(16) << Time << "   " << timeInc << "   " << Step << "   " << nexttime << endl;
          }
        }
        for (int k = 0; k < esize; k++) {
          // Check the length of 1 line
          if (k == 0) {
            lineLength = sizeof(int) + numVar * sizeof(double);
            buffer = new char[lineLength];
          }
          // Read local file
          localResultFile[process[k]].seekg((localID[k]-1) * lineLength + offsetInt, localResultFile[process[k]].beg);
          localResultFile[process[k]].read(reinterpret_cast<char *>(&data1),sizeof(int));
          for (int j = 0; j < numVar; j++)
            localResultFile[process[k]].read(reinterpret_cast<char *>(&data[j]),sizeof(double));

          // Write global file
          ID = k + 1;
          if (outBinary == 1) {
            globalResultFile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
            for (int j = 0; j < numVar; j++)
              globalResultFile.write(reinterpret_cast<const char *>(&data[j]),sizeof(double));
          } else {
            globalResultFile << setiosflags(ios::right) << setw(8) << k+1 << setiosflags (ios::fixed | ios::showpoint) ;
            for (int k = 0; k < numVar; k++)
              globalResultFile  << setw(20) << setprecision(13) << data[k];
            globalResultFile << endl;
          }
        }

        // Close all files and reset variables
        file = temp2;
        globalResultfilename = temp2;
        globalResultFile.close();
        for (int r = 0; r < numberofProcesses; r++)
          localResultFile[r].close();
        cout << "Done!\n";
      }
      break;

    default: // ASCII input
      for (int m = startNumber; m < endNumber; m+=interval) {
        sprintf(ftemp,"%8.8d",m);
        file.append(ftemp);
        if (mergingType)
          filelocal = "LM";
        file.append(filelocal);
        temp = file;
        globalResultfilename.append(ftemp);
        if (outBinary == 0)
          globalResultfilename.append(filetemp);
        else
          globalResultfilename.append(filetemp2);
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
        if (outBinary == 1)
          globalResultFile.open(globalResultfilename.c_str(), std::ofstream::out | ios::binary);
        else
          globalResultFile.open(globalResultfilename.c_str(), std::ofstream::out);
        
        if (mergingType) {
          localResultFile[0] >> Time >> timeInc >> Step >> nexttime;
          offsetInt = 68; // May not correct for 3D Intermediate files;
          if (outBinary == 1) {
            globalResultFile.write(reinterpret_cast<const char *>(&Time), sizeof(double));
            globalResultFile.write(reinterpret_cast<const char *>(&timeInc), sizeof(double));
            globalResultFile.write(reinterpret_cast<const char *>(&Step), sizeof(int));
            globalResultFile.write(reinterpret_cast<const char *>(&nexttime), sizeof(double));
          } else {
            globalResultFile << setiosflags(ios::right) << setiosflags (ios::fixed | ios::showpoint) << setw(24) << setprecision(16) << Time << "   " << timeInc << "   " << Step << "   " << nexttime << endl;
          }
        }
        if (outBinary == 0) {
          for (int k = 0; k < esize; k++) {
            // Check the length of 1 line
            if (k == 0) {
              getline(localResultFile[0], wholelinedata);
              if (mergingType) {
                int header = localResultFile[0].tellg();
                getline(localResultFile[0], wholelinedata);
                int headerEnd = localResultFile[0].tellg();
                lineLength = headerEnd - header;
              } else
                lineLength = localResultFile[0].tellg();
            }
            // Read local file
            localResultFile[process[k]].seekg((localID[k]-1) * lineLength + offset + offsetInt, localResultFile[process[k]].beg);
            if (mergingType)
              localResultFile[process[k]] >> dummyInt;
            getline(localResultFile[process[k]], wholelinedata);
            // Write global file
            globalResultFile << setiosflags(ios::right) << setw(8) << k+1 << wholelinedata << "\n";
          }
        } else {
          for (int k = 0; k < esize; k++) {
            // Check the length of 1 line
            if (k == 0) {
              getline(localResultFile[0], wholelinedata);
              if (mergingType) {
                int header = localResultFile[0].tellg();
                getline(localResultFile[0], wholelinedata);
                int headerEnd = localResultFile[0].tellg();
                lineLength = headerEnd - header;
              } else
                lineLength = localResultFile[0].tellg();
            }
            // Read local file
            localResultFile[process[k]].seekg((localID[k]-1) * lineLength, localResultFile[process[k]].beg);
            localResultFile[process[k]] >> data1;
            for (int j = 0; j < numVar; j++) {
              localResultFile[process[k]] >> data[j];
            }

            // Write global file
            ID = k + 1;
            globalResultFile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
            for (int j = 0; j < numVar; j++)
              globalResultFile.write(reinterpret_cast<const char *>(&data[j]), sizeof(double));
          }
        }

        // Close all files and reset variables
        file = temp2;
        globalResultfilename = temp2;
        globalResultFile.close();
        for (int r = 0; r < numberofProcesses; r++)
          localResultFile[r].close();
        cout << "Done!\n";
      }
      break;
  }

//  delete[] localResultFile;
//  delete[] buffer;
  return 0;
}
