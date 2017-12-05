/*
	Input 1: image file
	Input 2: structuring element
	Input 3: operation file
	input 4: outputfile
*/

#include <iostream>
#include <fstream> // class necessary to read files
using namespace std;

class singleMorphOp {
		
		private:
		
			int numRowsIMG, numColsIMG, minValIMG, maxValIMG;
	  		int numRowsStrctElem, numColsStrctElem, minValStrctElem, maxValStrctElem;
	  		int rowOrigin, colOrigin;
			int rowFrameSize, colFrameSize;
			int** imgAry;	
			int** backupImgAry;
			int** morphAry;	
	     	int** structElem;
			char* inFile1;	// image file
			char* inFile2;	// structuring element
			char* outFile1;	// outputfile
	
	    public:
	    	
			singleMorphOp (char* inFile1, char* inFile2, char* outFile1) {
								
				this->inFile1 = inFile1;
				this->inFile2 = inFile2;
				this->outFile1 = outFile1;
				
				loadStruct();
				computeFrameSize();
				loadImage();
		
				buildMorphAry();
				zeroOutMorphAry();
				
			} 
	
			void loadStruct() {
			
				ifstream myfile(inFile2);
				
				myfile >> numRowsStrctElem;
				myfile >> numColsStrctElem;
				myfile >> minValStrctElem;
				myfile >> maxValStrctElem;
				
				myfile >> rowOrigin;
				myfile >> colOrigin;
				
				structElem = new int*[numRowsStrctElem];
				for(int i = 0; i < numRowsStrctElem; i++)
					structElem[i] = new int[numColsStrctElem];
		 
		  		for(int i = 0; i < numRowsStrctElem; i++) {
		  			for(int j = 0; j < numColsStrctElem; j++)
		      			myfile >> structElem[i][j];
				}
				
				myfile.close(); // close input file
			
			}
			
			void computeFrameSize() {
				rowFrameSize = numRowsStrctElem;
				colFrameSize = numColsStrctElem;
			}
			
			void loadImage() {
				
				ifstream myfile(inFile1);
				
				myfile >> numRowsIMG;
				myfile >> numColsIMG;
				myfile >> minValIMG;
				myfile >> maxValIMG;
				
				imgAry = new int*[numRowsIMG + rowFrameSize];
				
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					imgAry[i] = new int[numColsIMG + colFrameSize];
		 
		 		backupImgAry = new int*[numRowsIMG + rowFrameSize];
				
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					backupImgAry[i] = new int[numColsIMG + colFrameSize];
		 
		 		zeroFrameImage();
		 
		  		for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
		  			for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						myfile >> imgAry[i][j];
						backupImgAry[i][j] = imgAry[i][j];
					}
				}
							
				myfile.close();
			}
			
			void zeroFrameImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
					  	imgAry[i][j] = 0;
					  	backupImgAry[i][j] = 0;
					}
				}
			}
			
			void buildMorphAry() {
				morphAry = new int*[numRowsIMG + rowFrameSize];
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					morphAry[i] = new int[numColsIMG + colFrameSize];
			}
			
			void zeroOutMorphAry() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++)
		      			morphAry[i][j] = 0;
				}
			}
			
			void processDilation() {
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						if (imgAry[i][j] > 0) {
							dilation(i, j);
						}
					}
				}
			}

			void dilation(int row, int col) {
				
				for(int imgRow = row-rowOrigin, strctRow = 0; 
						strctRow < numRowsStrctElem; 
						imgRow++, strctRow++) {
		  			
					for(int imgCol = col-colOrigin,	strctCol = 0;
							strctCol < numColsStrctElem;
							imgCol++, strctCol++) {
								
							if (structElem[strctRow][strctCol] > 0) {
		      					morphAry[imgRow][imgCol] = 1;
							}
					}
				}
			}
			
			
			void processErosion() {
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						if (imgAry[i][j] > 0) {
							erosion(i, j);
						}
					}
				}
			}
			
			void erosion(int row, int col) {
				
				bool flag = true;

				for(int imgRow = row-rowOrigin, strctRow = 0; 
						strctRow < numRowsStrctElem; 
						imgRow++, strctRow++) {
		  			
					for(int imgCol = col-colOrigin,	strctCol = 0;
							strctCol < numColsStrctElem;
							imgCol++, strctCol++) {
								
							if (structElem[strctRow][strctCol] == 1 && imgAry[imgRow][imgCol] != 1) {
		      					flag = false;
		      					break;
							}
					}	
				}
				if(flag) morphAry[row][col] = 1;
			}
			
			void updateInputImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						imgAry[i][j] = morphAry[i][j];
					}		
				}
			}
			
			void restoreInputImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						imgAry[i][j] = backupImgAry[i][j];
					}		
				}
			}
			
			void processClosing() {
				processDilation();
				updateInputImage();
				zeroOutMorphAry();
				processErosion();
				restoreInputImage();
			}
		
			void processOpening() {
				processErosion();
				updateInputImage();
				zeroOutMorphAry();
				processDilation();
				restoreInputImage();
			}
			
			void prettyPrintImgAry() {
				
				cout << "Input Image" << endl;
				cout << "===========" << endl;
				
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						if (imgAry[i][j] > 0) cout << imgAry[i][j] << " ";
						else cout << "  ";
					}
					cout << endl;
				}
				cout << endl;
			}
			
			void prettyPrintStructElem() {

				cout << "Structuring Element" << endl;
				cout << "===================" << endl;
								
				for(int i = 0; i < numRowsStrctElem; i++) {
		  			for(int j = 0; j < numColsStrctElem; j++) {
		  				if (structElem[i][j] > 0) cout << structElem[i][j] << " ";
						else cout << "  ";
					}
		      		cout << endl;
				}
				cout << endl;
			}
			
			void prettyPrintMorphAry(string result) {
				
				cout << result << endl;
				cout << "===============" << endl;
				
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						if (morphAry[i][j] > 0) cout << morphAry[i][j] << " ";
						else cout << "  ";
					}
					cout << endl;
				}
				cout << endl;
				
			}
			
			void outPutResult() {
				
				ofstream myoutfile(outFile1);
				
				myoutfile << numRowsIMG << " " << numColsIMG << " " << minValIMG << " " << maxValIMG << endl;
				
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						myoutfile << morphAry[i][j] << " ";
					}
					myoutfile << endl;
				}			
				myoutfile.close();
			}
			
};

int main(int argc, char *argv[]) {

	singleMorphOp m(argv[1], argv[2], argv[4]);
	
	char op;
	ifstream myfile(argv[3]);				
	myfile.get(op);
	myfile.close();
	
	switch(op) {
		case 'D' :
			m.processDilation(); 
			break;
		case 'E' :
			m.processErosion();
			break;
		case 'O' : 
			m.processOpening();
			break;
		case 'C' :
			m.processClosing();
			break;
		default :
			cout << "Invalid Operation" << endl;
   }
	
	m.outPutResult();
	m.zeroOutMorphAry();

	return 0;

} // end of main



