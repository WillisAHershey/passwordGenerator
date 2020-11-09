//Willis A. Hershey
//Random password generator
//Fourteenth of September, year of our Lord Two Thousand and Twenty

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNIX_ENTROPY_FILE "/dev/random"

//When this argument is read, the program expects the next value to be a valid integer
#define LENGTH_FLAG "-l"

//When this argument is read, the program expects the next argument to be "lower", "upper", or "mixed"
#define CASE_FLAG "-c"
#define LOWERCASE_ONLY "lower"
#define UPPERCASE_ONLY "upper"
#define MIXED_CASE "mixed"

//These are the values associated with the case setting used in the implementation
#define invalid 0
#define lower 1
#define upper 2
#define mixed 3
//(Notice mixed = lower | upper) ;)

//When this argument is read, the program adds special characters to the alphabet of the password
#define SPECIAL_CHARACTER_FLAG "-s"

//When this argument is read, the program adds numerical characters to the alphabet of the password
#define NUMBER_FLAG "-n"

//When this flag is read, the program will not attempt to use the UNIX entropy file to produce random values
#define USE_STDIO_RANDOM_FLAG "-stdio_rand"

//When this flag is read, the program expects the next argument to be a valid filename that can be written to
#define WRITE_TO_FLAG "-w"

//These are the error flags which can be or-ed together to get compound errors
#define NO_LENGTH_ERROR 0x1
#define NO_CASE_ERROR 0x2
#define BAD_WRITE_TO_ERROR 0x4

//These are all the supported characters to be included in the alphabet of the password
const char lowers[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
const char uppers[]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
const char numbers[]={'0','1','2','3','4','5','6','7','8','9'};
const char specials[]={'!','\"','#','$','%','&','\'','(',')','*','+',',','-','.','/',':',';','<','=','>','?','@','[','\\',']','^','_','`','{','|','}','~'};

//This is a struct which neatly and compactly stores the settings of this run of the program. On most systems this will be an 8-byte structure
typedef struct{
  unsigned int length;
  unsigned int cs:2;
  unsigned int specials:1;
  unsigned int numbers:1;
  unsigned int stdio:1;
  unsigned int errorCode:3;
}setting;

//inputError() receives an integer somewhere between 0 and 7 and prints the appropriate errors to stderr. If an error prints, the program exits with EXIT_FAILURE.
void inputError(int errorCode){
  if(!errorCode)
	return;
  if(errorCode&NO_LENGTH_ERROR)
	fprintf(stderr,"Input has either an invalid length argument or none at all. Include \"-l\" followed by a valid integer in the argument list and do not repeat flag\n");
  if(errorCode&NO_CASE_ERROR)
	fprintf(stderr,"Input has either an invalid case argument or none at all. Include \"-c\" followed by one of {\"lower\",\"upper\",\"mixed\"} in the argument list and do not repeat the flag\n");
  if(errorCode&BAD_WRITE_TO_ERROR)
	fprintf(stderr,"Unable to open filename following \"-w\" for writing, or multiple ouputs given. Please provide one filename that can be written to\n");
  exit(EXIT_FAILURE);
}

//This function will use the UNIX entropy file to produce random unless the fp is NULL, in which case it defaults to the stdio.h pseudo-random function.
unsigned int passwordRandom(FILE *f){
  static int seed=0;
  if(!f){
	if(!seed)
		srand((seed=time(NULL)));
	return (unsigned int)rand();
  }
  else{
	unsigned int out;
	//If fread() fails we get a rvalue of 0, which tells the program to just use stdio rand()
	if(!fread((void*)&out,sizeof(int),1,f))
		return passwordRandom(NULL);
	return out;
  }
}

int main(int args,char *argv[]){
  //Default output file is stdout, but this changes if "-w" is read
  FILE *write_to=stdout;
  //Necessary parameters are set to invalid values, and unnecessary parameters are set to default values
  setting s=(setting){.length=0,.cs=invalid,.specials=0,.numbers=0,.stdio=0,.errorCode=0};
  //This loop goes thorugh all the arguments and alters the settings of this run, and sets bits of error codes if something is wrong.
  for(int i=1;i<args;++i){
	if(!strcmp(LENGTH_FLAG,argv[i])){
		if(s.length) //Error if case is double set
			inputError(NO_LENGTH_ERROR);
		s.length=atoi(argv[++i]);
		if(!s.length)
			s.errorCode|=NO_LENGTH_ERROR; //Errors are set with | instead of ^ on the off chance the same error occurs an even number of times.
	}
	else if(!strcmp(CASE_FLAG,argv[i])){
		if(s.cs!=invalid) //Error if case is double-set
			inputError(NO_CASE_ERROR);
		++i;
		if(!strcmp(LOWERCASE_ONLY,argv[i]))
			s.cs=lower;
		else if(!strcmp(UPPERCASE_ONLY,argv[i]))
			s.cs=upper;
		else if(!strcmp(MIXED_CASE,argv[i]))
			s.cs=mixed;
		else
			s.errorCode|=NO_CASE_ERROR;
	}
	else if(!strcmp(SPECIAL_CHARACTER_FLAG,argv[i]))
		s.specials=1;
	else if(!strcmp(NUMBER_FLAG,argv[i]))
		s.numbers=1;
	else if(!strcmp(USE_STDIO_RANDOM_FLAG,argv[i]))
		s.stdio=1;
	else if(!strcmp(WRITE_TO_FLAG,argv[i])){
		if(write_to!=stdout) //Error if write_to is double-set
			inputError(BAD_WRITE_TO_ERROR);
		else{
			write_to=fopen(argv[++i],"w");
			if(!write_to){
				s.errorCode|=BAD_WRITE_TO_ERROR;
				write_to=stdout;
			}
		}
	}
	else
		fprintf(stderr,"Argument %s not understood, but will be ignored\n",argv[i]);
  }
  //If necessary parameters were never set, then set error codes
  if(!s.length)
	s.errorCode|=NO_LENGTH_ERROR;
  if(s.cs==invalid)
	s.errorCode|=NO_CASE_ERROR;
  //This function call returns with no effect if no error bits are set
  inputError(s.errorCode);
  //If stdio bit is set, then f is set to NULL, otherwise it attempts to open UNIX entropy file. If that fails, then f is NULL anyway.
  FILE *f=s.stdio?NULL:fopen(UNIX_ENTROPY_FILE,"rb");
  //We calculate the total size of the alphabet based on the settings
  int alphabet=0;
  if(s.cs&lower)
	alphabet+=sizeof lowers;
  if(s.cs&upper)
	alphabet+=sizeof uppers;
  if(s.specials)
	alphabet+=sizeof specials;
  if(s.numbers)
	alphabet+=sizeof numbers;
  //Then we loop though producing random integers between zero and sizeof(alphabet)-1
  for(int i=0;i<s.length;++i){
	//passwordRandom() uses stdio rand() if reading from /dev/random fails
	int index=passwordRandom(f)%alphabet;
	if(s.cs&lower){
		if(index<sizeof(lowers)){
			fprintf(write_to,"%c",lowers[index]);
			continue;
		}
		else
			index-=sizeof(lowers);
	}
	if(s.cs&upper){
		if(index<sizeof(uppers)){
			fprintf(write_to,"%c",uppers[index]);
			continue;
		}
		else
			index-=sizeof(uppers);
	}
	if(s.numbers){
		if(index<sizeof(numbers)){
			fprintf(write_to,"%c",numbers[index]);
			continue;
		}
		else
			index-=sizeof(numbers);
	}
	if(s.specials){
		if(index<sizeof(specials)){
			fprintf(write_to,"%c",specials[index]);
			continue;
		}
		else{ //This should be impossible. I'm fairly certain this can never happen.
			fprintf(stderr,"Something went wrong :/\n");
			exit(EXIT_FAILURE);
		}
	}
  }
  fprintf(write_to,"\n");
  //If write_to is manually set to some file, we make sure we close it
  if(write_to!=stdout)
	fclose(write_to);
  //And if we were reading from the UNIX entropy file we wanna close that too.
  if(f)
	fclose(f);
  //And we're done :)
  exit(EXIT_SUCCESS);
}
