#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define TRUE  1
#define FALSE 0

#define DEBUG 1     // 1 => Turn ON debugging, 0 => Turn OFF debugging

void print_debug(const char * format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}

//----------------------------- token types ------------------------------
#define KEYWORDS 	12
#define RESERVED 	38
#define VAR 		1
#define BEGIN 		2
#define END 		3
#define ASSIGN 		4
#define IF 			5
#define WHILE 		6
#define DO 			7
#define THEN 		8
#define PRINT 		9
#define INT 		10
#define REAL 		11
#define STRING 		12
#define PLUS 		13
#define MINUS 		14
#define UNDERSCORE 	15
#define DIV 		16
#define MULT 		17
#define EQUAL 		18
#define COLON 		19
#define COMMA 		20
#define SEMICOLON 	21
#define LBRAC 		22
#define RBRAC 		23
#define LPAREN 		24
#define RPAREN 		25
#define NOTEQUAL 	26
#define GREATER 	27
#define LESS 		28
#define LTEQ 		29
#define GTEQ 		30
#define LSHIFT 		31
#define RSHIFT 		32
#define DOT 		33
#define NUM 		34
#define INTID 		35
#define REALID 		36
#define ID 			37
#define ERROR 		38

//------------------- reserved words and token strings -----------------------
char *reserved[] =
	{	"",
		"VAR",
		"BEGIN",
		"END",
		"ASSIGN",
		"IF",
		"WHILE",
		"DO",
		"THEN",
		"PRINT",
		"INT",
		"REAL",
		"STRING",
		"+",
		"-",
		"_",
		"/",
		"*",
		"=",
		":",
		",",
		";",
		"[",
		"]",
		"(",
		")",
		"<>",
		">",
		"<",
		"<=",
		">=",
		"<<",
		">>",
		".",
		"NUM",
		"INTID",
		"REALID",
		"ID",
		"ERROR"
		};

int printReserved(int ttype)
{
	if (ttype <= RESERVED)
	{
		printf("%s\n",reserved[ttype]);
		return 1;
	}
	else
		return 0;
}
//---------------------------------------------------------

//---------------------------------------------------------
// Global Variables associated with the next input token
// This implementation does not allow tokens that are more
// than 100 characters long
#define MAX_TOKEN_LENGTH 100

char token[MAX_TOKEN_LENGTH];     // token string
int  tokenLength;
int  line_no = 0;

//---------------------------------------------------------
// Global Variables associated with the input buffer
// This implementation does not allow lines that are more
// than 500 characters long
#define BUFFER_SIZE     500

int buffer[BUFFER_SIZE];
int start_pos   = 0;	// Signifies the start position in the buffer (#)
int next_pos    = 0;	// Signifies the next character to be read (^)
int maximal_pos = 0;	// Signifies the last character of the maximal 
                        // token found (*)
int temp_pos = 0;
//---------------------------------------------------------
// Functions associated with the input buffer

void readNextLine()
{
	// Reads the entered line fully

	int  i;
	char c;
	start_pos   = -1; // start position is the position just
                      // before the beginning of the token string
	maximal_pos = -1; // maximal_position points to the end of the
                      // maximal_matched token string
	next_pos    = 0;  // next_pos points to the next input character

	temp_pos = 0;

	i = 0;
	c = getchar();
	while (c != '\n' && !feof(stdin) && (i < BUFFER_SIZE))
	{
		buffer[i] = c;
		c = getchar();
		i++;
	}
	if (i >= BUFFER_SIZE)
	{
		printf("Maximum allowed line length exceeded\n");
		exit(1);
	}
	else
		buffer[i] = EOF;
	line_no++;
}

/* you can use the following function for Debugging purposes */
void printBuffer()
{
	print_debug("BUFFER: ");
	int i = 0;
	while (buffer[i] != EOF)
	{
		print_debug("%c", (char) buffer[i]);
		i++;
	}
	print_debug("\n");
	print_debug("START_POS   = %d\n", start_pos);
	print_debug("NEXT_POS    = %d\n", next_pos);
	print_debug("MAXIMAL_POS = %d\n", maximal_pos);
}

int skip_space_and_set_positions()
{
	while (isspace((char) buffer[next_pos]))
	{
		next_pos++;
	}
	maximal_pos = next_pos - 1;
	start_pos   = next_pos - 1;
}

// this function copies the maximal token string to the token array
// it assumes the token string is not empty
void copy_token_string_from_buffer()
{
	int i;
	tokenLength = 0;
	for (i = start_pos + 1; i <= maximal_pos; i++)
	{
		token[tokenLength] = (char) buffer[i];
		tokenLength++;
	}
	token[tokenLength] = '\0';
}

//----------------------------------------------------------

// This function checks if a string is a keyword. 
// The implementation is not the most efficient, but probably 
// the simplest!
int isKeyword(char *s)
{
	int i;

	for (i = 1; i <= KEYWORDS; i++)
		if (strcmp(reserved[i],s) == 0)
	   		return i;
	return FALSE;
}


int isDot()
{
	if (buffer[temp_pos] == '.')
		return 1;
	else 
		return 0;

}

int isReal()
{
	int i,j;
	char* real_kwd = "REAL";
	for(i=temp_pos,j=0;j<4;i++,j++)
		{
			if (buffer[i] != real_kwd[j])
				{
					return 0;
				}
		}
		temp_pos += 4;
		return 1;
}

int isInt()
{
	int i,j;
	char* int_kwd = "INT";
	for(i=temp_pos,j=0;j<3;i++,j++)
		{
			if (buffer[i] != int_kwd[j])
				{
					return 0;
				}
		}
		temp_pos += 3;
		return 1;
}

int isRealorInt()
{
	int num_flag = 0;
	temp_pos = next_pos;

	if (isDot())
		{
			temp_pos++;
			if (buffer[temp_pos] == '0' && buffer[temp_pos +1] == '0')
				return 0;
			while(isdigit(buffer[temp_pos]))
			{
				temp_pos++;
				num_flag = 1;
			}
			if (num_flag)
				{	
					if (isReal())
						{
							next_pos = temp_pos;
							return REALID;
						}
					else if (isInt())
						{
							next_pos = temp_pos;
							return INTID;
						}
					else 
						return 0;
				}
			else 
				return 0;
		}
}

int scan_id_keyword()
{
	int ttype, temptype;

	while (isalnum(buffer[next_pos]))
	{
		next_pos++;
	}

	temptype = isRealorInt();

	maximal_pos = next_pos - 1;

	
	copy_token_string_from_buffer();

	ttype = isKeyword(token);
	if (ttype == 0)
		if (temptype==0)
			ttype = ID;
		else
			ttype = temptype;
	return ttype;
}

// this function is called only if the next input character is a digit
int scan_number()
{
	int ttype;

	ttype = NUM;
	if (buffer[next_pos] != '0')
		while (isdigit(buffer[next_pos]))
			next_pos++;
	else // NUM is 0
		next_pos++;


	maximal_pos = next_pos - 1;

	copy_token_string_from_buffer();
	return ttype;
}

int getToken()
{
	char c;
	int ttype;

	skip_space_and_set_positions();

	c = buffer[next_pos];
	next_pos++;

	switch (c)
	{
		case '.': ttype = DOT; break;
		case '+': ttype = PLUS; break;
		case '-': ttype = MINUS; break;
		case '_': ttype = UNDERSCORE; break;
		case '/': ttype = DIV; break;
		case '*': ttype = MULT; break;
		case '=': ttype = EQUAL; break;
		case ':': ttype = COLON; break;
		case ',': ttype = COMMA; break;
		case ';': ttype = SEMICOLON; break;
		case '[': ttype = LBRAC; break;
		case ']': ttype = RBRAC; break;
		case '(': ttype = LPAREN; break;
		case ')': ttype = RPAREN; break;
		case '<':
			/*
			 * TODO: Write code to handle <= and << here
			 */
			if (buffer[next_pos] == '=')
			{
				ttype = LTEQ;
				next_pos++;
			}
			else if (buffer[next_pos] == '<')
			{
				ttype = LSHIFT;
				next_pos++;
			}
			else if (buffer[next_pos] == '>')
			{
				ttype = NOTEQUAL;
				next_pos++;
	
			}
			else
				ttype = LESS;
			break;
		case '>':
			/*
			 * TODO: Wriet code to handle >= and >> here
			 */
			if (buffer[next_pos] == '=')
			{
				ttype = GTEQ;
				next_pos++;
			}
			else if(buffer[next_pos] == '>')
			{
				ttype = RSHIFT;
				next_pos++;
			}
			else
				ttype = GREATER;
			break;
		default :
			if (isdigit(c))
			{
				next_pos--;
				ttype = scan_number();
			}
			else if (isalpha(c)) 
			{
				// token is either a keyword or ID or INTID or REALID
				next_pos--;
				ttype = scan_id_keyword();
				/* 
				 * TODO: Write code to handle INTID and REALID
				 * It might be better to replace scan_id_keyword() 
				 * with a function that handles INTID and REALID 
				 * in addition to ID and keywords
				 */
			}
			else if (c == EOF)
			  ttype = EOF;
			else
			  ttype = ERROR;
			break;
	}	// End Switch
	return ttype;
}

int main()
{
	int ttype;

	while (!feof(stdin))
	{
		readNextLine();
		// printBuffer();
		while ((ttype = getToken()) != EOF)
		{
			/*  EOF indicates that we have reached the end of buffer */
			// printBuffer();
			printf("%d %d ", line_no, ttype);
			if ((ttype == NUM) || (ttype == ID) || 
				(ttype == INTID) || (ttype == REALID))
			{
				printf("%s \n", token);
			}
			else
			{
				printReserved(ttype);
			}
		}
	}
	return 0;
}

