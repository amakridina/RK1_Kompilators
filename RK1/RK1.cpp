#include <cctype>
#include <iostream>
#include <map>
#include <list>
#include <sstream>
#include <string>

using namespace std;

enum Token_value : char //тут наши операции
{
  NEW,				NAME,					NUMBER,         END,
  PLUS='+',         MINUS='-',              MUL='*',        DIV='/',
  EQUAL='=',		LESS='<',				GREATER='>',
  LESSEQ,			GREATEREQ,				NOEQUAL, 
  AND='&',			OR='|',
  SET,				GOTO,					
  PRINT,			PRINTSTEP,				PRINTSTEPS,
  CLEAR,			RESET,					DELETESTEP,		ERR
};

enum Number_value : char //цифры, которые можем использовать
{
  NUM0='0', NUM1='1', NUM2='2',
  NUM3='3', NUM4='4', NUM5='5',
  NUM6='6', NUM7='7', NUM8='8',
  NUM9='9', NUMP='.',
};

Token_value curr_tok = PRINT;        // Хранит последний возврат функции get_token(). 
double number_value;                 // Хранит целый литерал(с плавающей запятой).
string string_value;            // Хранит имя.
map <string, double> variable; // словарь переменных со знаением.
map <string, string> step; // словарь для шагов
list <string> list_step; //список последовательных шагов
int no_of_errors;                    // Хранит количество встречаемых ошибок.

double expr(istream*, bool);    // Обязательное объявление.


// Функция error() имеет тривиальный характер: инкрементирует счётчик ошибок.
double error(const std::string& error_message) {
  ++no_of_errors;
  cerr << "error: " << error_message << endl;
  return 1;
}

Token_value get_token(std::istream* input) {
  char ch;

  do {    // Пропустить все пробельные символы кроме '\n'.
    if (!input->get(ch)) {
      return curr_tok = END; // Завершить работу калькулятора.
    }
  } while (ch != '\n' && isspace(ch));

  switch (ch) {
    case 0: // При вводе символа конца файла, завершить работу калькулятора.
      return curr_tok = END;
    case '\n':
      return curr_tok = NEW;
    case MUL:
    case DIV:
    case PLUS:
    case MINUS:
    case EQUAL:
	case LESS:
	case GREATER:
      return curr_tok = Token_value(ch); // Приведение к целому и возврат.
    case NUM0: case NUM1: case NUM2: case NUM3: case NUM4:
    case NUM5: case NUM6: case NUM7: case NUM8: case NUM9:
    case NUMP:
      input->putback(ch); // Положить назад в поток...
      *input >> number_value; // И считать всю лексему.
      return curr_tok = NUMBER;
    default:

      if (isalpha(ch)) {//проверяем что это буква, тогда это может быть переменной		
		  // тут пытаемся запомнить команду
		string str;
		str = ch;
		char ch2;
		while (input->get(ch2) && isalnum(ch2)) 
			str.push_back(ch2);
		list_step.push_back(str);

		input->putback('\n');
		for(int i=str.size()-1;i>=0 ; i--)
			input->putback(str.at(i));


		string_value.clear();
        while (input->get(ch) && isalnum(ch)) {
          string_value.push_back(ch);
        }
        input->putback(ch);

		if (string_value == "SET") 
			return curr_tok = SET;
		if (string_value ==  "PRINT")
			return curr_tok = PRINT;
		if (string_value ==  "PRINTSTEP")
			return curr_tok = PRINTSTEP;
		if (string_value ==  "PRINTSTEPS")
			return curr_tok = PRINTSTEPS;
		if (string_value ==  "CLEAR")
			return curr_tok = CLEAR;
		if (string_value ==  "RESET")
			return curr_tok = RESET;
		if (string_value ==  "DELETESTEP")
			return curr_tok = DELETESTEP;
		else
			return curr_tok = NAME;
      }
      error("Bad Token");
      return curr_tok = PRINT;
  }
}

double variable_token(std::istream* input)
{
	double v = 1;
	return v;
}
//Каждая функция синтаксического анализа принимает аргумент типа bool
//  указывающий, должна ли функция вызывать get_token() для получения
//  очередной лексемы. 

// prim() - обрабатывает первичные выражения.
double prim(std::istream* input, bool get) {
  if (get) {
    get_token(input);
  }

  //SET='set',		GOTO="goto",			PRINT='print',  
  //PRINTSTEP='printstep',					PRINTSTEPS='printsteps',
  //CLEAR='clear',	RESET='reset',			DELETESTEP='deletestep',
  switch (curr_tok)  {
    case GOTO: {
		break;
	}
	case NUMBER: {
      double& v = variable[string_value];
      if (get_token(input) == EQUAL) {
         // v = expr(input, true);
      }
      return v;
    }
	case SET: {
		if (get_token(input) == NAME && string_value.size() == 1) {
			cout<<"yes"<<endl;
			
		}
		break;
	}
	case PRINT: {
		break;
	}
	case PRINTSTEP: {
		if (!list_step.empty())
		{
			list_step.pop_back();
			cout << list_step.back()<<endl;
			list_step.push_back("PRINTSTEP");
		}
		else 
			cout << "steps list is empty";
		break;
	}
	case PRINTSTEPS: {
		ostream_iterator<string> os_out (cout,"\n");
		//copy(list_step.begin(), list_step.end(), os_out);
		list<string>::iterator it = list_step.begin();
		list<string>::iterator it2 = list_step.begin();
		for (it++; it!= list_step.end(); ++it)
		{
			cout  << *it2 << endl;
			it2++;
		}
		break;
	}
	case CLEAR: {
		variable.clear();
		break;
	}
	case RESET:	{
		variable.clear();
		step.clear();
		list_step.clear();
		break;
	}
	case DELETESTEP:{
		list_step.pop_back();
		break;
	}

    /*case MINUS:
		
      return -prim(input, true);
    case LP: {
      double e = expr(input, true);
      if (curr_tok != RP) {
          return error("')' expected");
      }
      get_token(input);
      return e;
    }*/
    default:
      return error("primary expected");
  }
}

// term() - умножение и деление.
/*double term(std::istream* input, bool get) {
  double left = prim(input, get);

  for ( ; ; ) {
    switch (curr_tok) {
      case MUL:
        left *= prim(input, true);
        break;
      case DIV:
        if (double d = prim(input, true)) {
            left /= d;
            break;
        }
        return error("Divide by 0");
      default:
          return left;
    }
  }
}*/

// expr() - сложение и вычитание.
/*double expr(std::istream* input, bool get) {
  double left = term(input, get);

  for ( ; ; ) {
    switch (curr_tok) {
      case PLUS:
        left += term(input, true);
        break;
      case MINUS:
        left -= term(input, true);
        break;
      default:
        return left;
    }
  }
}*/

int main(int argc, char* argv[]) {
   istream* input;// = nullptr; // Указатель на поток.

  switch (argc) {
    case 1:
      input = &cin;
      break;
    case 2:
      input = new istringstream(argv[1]);
      break;
    default:
      error("Too many arguments");
      return 1;
  }

  while (*input) {
	  get_token(input);	
    if (curr_tok == END) {
      break;
    }

	if (curr_tok == NEW) {
		continue;
	}

	if (curr_tok == ERR)
	{
		continue;
	}

    // expr() -> term() -> prim() -> expr() ...
    prim(input, false);
  }

  if (input != &std::cin) {
    delete input;
  }

  return no_of_errors;
}