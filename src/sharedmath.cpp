/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
 
#include "sharedmath.h"
#include <string.h>

void quickSort( std::vector<LogicalCoordinate> *cloaArray, int nStartElement, int nEndElement )
{
	if ( nEndElement - nStartElement >= 15 ) //if we are sorting at least 16 elements
	{
		int nFirstArrayEnd = 0, nSecondArrayStart = 0;
		quickSortPartition( cloaArray, nStartElement, nEndElement, nFirstArrayEnd, nSecondArrayStart );
		
		if ( nFirstArrayEnd > nStartElement )
			quickSort( cloaArray, nStartElement, nFirstArrayEnd );
		
		if ( nSecondArrayStart < nEndElement )
			quickSort( cloaArray, nSecondArrayStart, nEndElement );
	}
}

void quickSortWrap( std::vector<LogicalCoordinate> *cloaArray )
{
	long lStartTime = clock();
	quickSort( cloaArray, 0, cloaArray->size() - 1 );

	//insertion sort
	for ( unsigned int nElement = 0; nElement < cloaArray->size(); nElement++ )
	{
		LogicalCoordinate cloaElement = cloaArray->at( nElement );
		int nCompare;
		
		for ( nCompare = nElement - 1; nCompare >= 0; nCompare-- )
		{
			if ( cloaArray->at( nCompare ).dLogicalParameter <= cloaElement.dLogicalParameter ) break;
			cloaArray->at( nCompare + 1 ) = cloaArray->at( nCompare ); //shift along
		}			
		cloaArray->at( nCompare + 1 ) = cloaElement;
	}

	long lEndTime = clock();
		
	std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to quicksort " << cloaArray->size() << " items\n";
}


int quickSortPruneSortedDuplications( std::vector<LogicalCoordinate> *cloaArray )
{
	unsigned int nWriter = 1, nElement; // write the 'pruned' elements over the top of the existing sorted array
	for ( nElement = 1; nElement < cloaArray->size(); nElement++ )
	{
		// if this co-ordinate is different from the previous
		if ( cloaArray->at( nElement ).dLogicalParameter > cloaArray->at( nElement - 1 ).dLogicalParameter )
		{
			cloaArray->at( nWriter ) = cloaArray->at( nElement );
			nWriter++;
		}
	}
	// resize the array so that only the newly written 'pruned' elements remain
	cloaArray->resize( nWriter );
	return nElement - nWriter;
}


void quickSortPartition( std::vector<LogicalCoordinate> *cloaArray, int nStartElement, int nEndElement, int &nFirstArrayEnd, int &nSecondArrayStart )
{
	if ( nStartElement < 0 || nEndElement >= int(cloaArray->size()) ) // range check
	{
		std::cout << "[DEBUG] ERROR: Invalid bounds to quickSortPartition" << std::endl;
	}
	else
	{
		int nLowEntry = nStartElement + 1, nPivotEntry = nStartElement + 1;
	
		int nRandomElements[3] = {
			randomInt( nStartElement, nEndElement ),
			randomInt( nStartElement, nEndElement ),
			randomInt( nStartElement, nEndElement )
		};
		
		// move the median valued index to nRandomElements[1]
		if ( cloaArray->at( nRandomElements[1] ).dLogicalParameter < cloaArray->at( nRandomElements[0] ).dLogicalParameter )
			swapv( nRandomElements[1], nRandomElements[0] );
		
		if ( cloaArray->at( nRandomElements[2] ).dLogicalParameter < cloaArray->at( nRandomElements[1] ).dLogicalParameter )
			swapv( nRandomElements[2], nRandomElements[1] );
		
		swapv( cloaArray->at( nStartElement ), cloaArray->at( nRandomElements[1] ) );
		float fPivotValue = cloaArray->at( nStartElement ).dLogicalParameter;
		
		for ( int nElement = nStartElement + 1; nElement <= nEndElement; nElement++ )
		{
			if ( cloaArray->at(nElement).dLogicalParameter < fPivotValue )
			{
				swapv( cloaArray->at( nElement ), cloaArray->at( nLowEntry ) );
				nLowEntry++;
			}
			else if ( cloaArray->at( nElement ).dLogicalParameter == fPivotValue )
			{
				swapv( cloaArray->at( nElement ), cloaArray->at( nLowEntry ) );
				swapv( cloaArray->at( nPivotEntry ), cloaArray->at( nLowEntry ) );
				nPivotEntry++;
				nLowEntry++;
			}
		}
		
		// put the pivot in the middle
		for ( int nPivotCopy = nStartElement; nPivotCopy < nPivotEntry; nPivotCopy++ )
		{
			swapv( cloaArray->at( nPivotCopy ), cloaArray->at( nLowEntry - ( 1 + nPivotCopy - nStartElement ) ) );
		}
		int nPivotSize = nPivotEntry - nStartElement;
		
		nFirstArrayEnd = nLowEntry - nPivotSize - 1;
		nSecondArrayStart = nLowEntry;
	}
}

// Apply a binary (two arguments) mathematical operation to a value and a term
void applyOperation( EvaluationValue &valModify, double dTermValue, int nCurrentOperation )
{
	switch ( nCurrentOperation )
	{
		case 1: valModify.value += dTermValue; break; // add
		case 2: valModify.value -= dTermValue; break; // subtract
		case 3: valModify.value *= dTermValue; break; // multiply
		case 4: valModify.value = pow( valModify.value, dTermValue ); break; // raise
		case 5: valModify.value /= dTermValue; break; //divide
	}
}

// no bodmas, operations are done as they are reached
EvaluationValue evaluateRawExpression( const char **szExpression, double dVariableValue )
{
	// return value
	EvaluationValue valReturn = { 0.0, false };
	
	double dTermValue = 0.0;
	int nCurrentOperation = 1, nNewOperation = 0; //start by adding, e.g. "24 * 4" is actually "0 + 24 * 4"
	bool bSeekingNewTerm = true;
	
	if ( **szExpression == '-' ) //then we should start by subtracting: -4 is the same as 0 - 4
	{
		(*szExpression)++;
		nCurrentOperation = 2;
	}
	
	while ( **szExpression != ')' ) // while we are not at the end of the expression
	{
		if ( **szExpression == ' ' ) // if end of expression reached but missing a closing bracket
		{
			std::cout << "Syntax error: Missing close bracket ) symbol!" << std::endl;
			(*szExpression)--; // move us back one character so that the any parents (recursive callers) can unwind by finding this space symbol
			
			// set the syntax error flag in the return value
			valReturn.value = NAN;
			valReturn.syntaxerror = true;
			break; // exit the main while loop
		}
		
		// get terms one by one
		if ( bSeekingNewTerm && ( isNumeric( **szExpression ) || **szExpression == '-' ) )
		{
			int nDigit = 0; // how many characters of the number we have read
			bool bWithinTerm = true;

			int nDecimalPoints = 0;
			
			while ( bWithinTerm ) // read in the number's digits
			{
				char cCurrentCharacter = *( *szExpression + nDigit );
				if ( !isNumeric( cCurrentCharacter ) )
				{
					switch ( cCurrentCharacter )
					{
						case '-':
							if ( nDigit > 0 )
								bWithinTerm = false;
							 
							break;
							
						case '.':
							nDecimalPoints++; // decimal points encountered in this term
							if ( nDecimalPoints > 1 )
							{
								std::cout << "[ERROR] Syntax error: You can't have two decimal points in a number!" << std::endl;
								valReturn.value = NAN;
								valReturn.syntaxerror = true;
								bWithinTerm = false;
							}
							break;
							
						default:
							bWithinTerm = false;
					}
				}
				nDigit++;
			}
			
			nDigit--;
			
			char cProceedingCharacter = *(*szExpression + nDigit); // the character after the numeric term
			bool bVariableCoefficientMode = false; // whether this character is the expression variable
			
			switch ( cProceedingCharacter )
			{
				case 'x':
				case 'y':
				case 'r':
				case 't':
				case 'o':
					bVariableCoefficientMode = true;
					break;
			}
			
			char szNumericTerm[ nDigit + 1 ];
			szNumericTerm[ nDigit ] = 0;
			strncpy( szNumericTerm, *szExpression, nDigit );
			
			if ( nDigit == 1 && szNumericTerm[ 0 ] == '-' )
			{
				if ( bVariableCoefficientMode )
					dTermValue = -1;
				else
				{
					std::cout << "[ERROR] Syntax error: A minus sign by itself is meaningless!";
					valReturn.value = NAN;
					valReturn.syntaxerror = true;
				}
			}
			else
			{
				dTermValue = atof( szNumericTerm ); //get numeric value of string
			}
			
			if ( bVariableCoefficientMode )
			{
				dTermValue *= dVariableValue; //use value as coefficient
				++*szExpression;
			}
			*szExpression += nDigit - 1; // move our current position in the expression along
			
			bSeekingNewTerm = false; // a term has been read
		}
		else
		{	
			int nFunction = 0; // the id of the function found
			if ( bSeekingNewTerm && *(*szExpression + 3) == '(' )
			{
				if      ( strncmp( *szExpression, "sin(", 4 ) == 0 )    nFunction = 1;
				else if ( strncmp( *szExpression, "cos(", 4 ) == 0 )    nFunction = 2;
				else if ( strncmp( *szExpression, "tan(", 4 ) == 0 )    nFunction = 3;
				else if ( strncmp( *szExpression, "log(", 4 ) == 0 )    nFunction = 4;
				else if ( strncmp( *szExpression, "abs(", 4 ) == 0 )    nFunction = 5;
				else if ( strncmp( *szExpression, "sec(", 4 ) == 0 )    nFunction = 6;
				else if ( strncmp( *szExpression, "csc(", 4 ) == 0 )    nFunction = 7;
				else if ( strncmp( *szExpression, "cot(", 4 ) == 0 )    nFunction = 8;
				else if ( strncmp( *szExpression, "fct(", 4 ) == 0 )    nFunction = 9;
			}
				
			if ( nFunction > 0 ) // if a function was found
			{
				*szExpression += 4; // move the expression read pointer to just after the (
				
				// szExpression is changed by the function call to point to the final ) !
				EvaluationValue valSubExpression = evaluateRawExpression( szExpression, dVariableValue ); // recursive call
				valReturn.syntaxerror |= valSubExpression.syntaxerror;
				double dExpressionValue = valSubExpression.value;
				
				switch (nFunction)
				{
					case 1: dTermValue = sin( dExpressionValue );           break;
					case 2: dTermValue = cos( dExpressionValue );           break;
					case 3: dTermValue = tan( dExpressionValue );           break;
					case 4: dTermValue = log( dExpressionValue );           break;
					case 5: dTermValue = fabs( dExpressionValue );          break;
					case 6: dTermValue = 1.0 / cos( dExpressionValue );     break;
					case 7: dTermValue = 1.0 / sin( dExpressionValue );     break;
					case 8: dTermValue = 1.0 / tan( dExpressionValue );     break;
					case 9: dTermValue = factorial( dExpressionValue );     break;
				}
				
				bSeekingNewTerm = false;
			}
			else
			{
				switch ( **szExpression )
				{
					case 'x': case 'y': case 'r': case 't': case 'o':
						if ( bSeekingNewTerm )
						{
							dTermValue = dVariableValue; // only one variable, so either x or y (or r/theta for polar)
							bSeekingNewTerm = false;
						}
						else
						{
							std::cout << "[ERROR] Syntax error: Operator expected, but found " << **szExpression << "\n";
							valReturn.value = NAN;
							valReturn.syntaxerror = true;
						}
						break;
							 
					case 'e':
						if ( bSeekingNewTerm )
							dTermValue = MATHCONST_E; // only one variable, so either x or y (or r/theta for polar)
						else
							std::cout << "[ERROR] Syntax error: Operator expected, but found " << **szExpression << "\n";
						valReturn.value = NAN;
						valReturn.syntaxerror = true;
						break;

					case 'p':
						if ( bSeekingNewTerm )
							dTermValue = MATHCONST_PI; // only one variable, so either x or y (or r/theta for polar)
						else
							std::cout << "[ERROR] Syntax error: Operator expected, but found " << **szExpression << "\n";
						valReturn.value = NAN;
						valReturn.syntaxerror = true;
						break;
						
					//operators complete current instruction...
					case '+': nNewOperation = 1; break;
					case '-': nNewOperation = 2; break;
					case '*': nNewOperation = 3; break;
					case '^': nNewOperation = 4; break;
					case '/': nNewOperation = 5; break;
					case '(': //open sub-expression
					{
						if ( !bSeekingNewTerm ) //then multiplication is implied, e.g. 3(x+2) = 3*(x+2)
						{
							applyOperation( valReturn, dTermValue, nCurrentOperation );
							nCurrentOperation = 3; // multiply
						}
						
						(*szExpression)++; // make pointer point to just after the opening bracket
						
						EvaluationValue valSubExpression = evaluateRawExpression( szExpression, dVariableValue ); //recursive call
						valReturn.syntaxerror |= valSubExpression.syntaxerror;
						dTermValue = valSubExpression.value;
						
						bSeekingNewTerm = false;
					}	
					break;
						
					default:
						std::cout << "[ERROR] Syntax error: Unrecognised character - " << **szExpression << "\n";
						valReturn.value = NAN;
						valReturn.syntaxerror = true;
				}
				
				if ( nNewOperation > 0 )
				{
					if ( bSeekingNewTerm )
					{
						std::cout << "[ERROR] Syntax error: Binary operator without two terms - " << **szExpression << "\n";
						valReturn.value = NAN;
						valReturn.syntaxerror = true;
					}
					
					bSeekingNewTerm = true; // there needs to be a term following this
					
					//complete current operation
					applyOperation( valReturn, dTermValue, nCurrentOperation );
					
					dTermValue = 0.0;
					nCurrentOperation = nNewOperation;
					nNewOperation = 0;
				}
			}
		}
		(*szExpression)++;
	}
	
	if ( bSeekingNewTerm )
	{
		std::cout << "[ERROR] Syntax error: Missing final term!\n";
	}
	else // complete final operation
	{	
		applyOperation( valReturn, dTermValue, nCurrentOperation );
	}
	
	return valReturn;
}
