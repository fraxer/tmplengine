#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <ctype.h>

using namespace std;

class CustomType {
	private:
		enum {CHAR, INT, DOUBLE, STRING, VECTOR} tag;
		void* p = nullptr;

	public:
		CustomType() {}

		CustomType(const CustomType& var) {
			
			if(var.tag == STRING) {
				tag = STRING;

				if(p != nullptr)
					deletePointer();

				p = new string(*(static_cast<string*>(var.p)));

				return;
			}

			if(var.tag == VECTOR) {
				tag = VECTOR;

				if(p != nullptr)
					deletePointer();

				p = new vector< map<string, string> >(*(static_cast<vector< map<string, string> >*>(var.p)));

				return;
			}
		}

		CustomType(string&& var) {
			// cout << "string" << endl;

			tag = STRING;

			if(p != nullptr)
				deletePointer();
			
			p = new string(var);
		}

		CustomType(const string& var) {
			// cout << "string" << endl;

			tag = STRING;

			if(p != nullptr)
				deletePointer();
			
			p = new string(var);
		}


		CustomType(vector< map<string, string> > var) {
			// cout << "vector" << endl;

			tag = VECTOR;

			if(p != nullptr)
				deletePointer();

			p = new vector< map<string, string> >(var);
		}

		CustomType& operator=(const CustomType& var) {

			if(var.tag == STRING) {
				tag = STRING;

				if(p != nullptr)
					deletePointer();

				p = new string(*(static_cast<string*>(var.p)));

				return *this;
			}

			if(var.tag == VECTOR) {
				tag = VECTOR;

				if(p != nullptr)
					deletePointer();

				p = new vector< map<string, string> >(*(static_cast<vector< map<string, string> >*>(var.p)));

				return *this;
			}

			return *this; // return the result by reference
		}

		string& getString() {
			return *(static_cast<string*>(p));
		}
		
		vector< map<string, string> >& getVector() {
			return *(static_cast<vector< map<string, string> >*>(p));
		}

		void deletePointer() {
			if(tag = STRING) {
				if(p != nullptr) {
					delete static_cast<string*>(p);
				}

				return;
			}

			if(tag = VECTOR) {
				if(p != nullptr)
					delete static_cast<vector< map<string, string> >*>(p);

				return;
			}

			return;
		}

		~CustomType() {

			deletePointer();
			
		}
};


class TemplateEngine {

	private:

		unsigned short for_deep;

		unsigned short if_deep;

		map<int, int> loopPosition;// номер цикла и его позиция

		map<int, string> loopData;// номер цикла и значение контейнера с данными

		map<string, CustomType>::iterator gl_values_it;

		map<string, CustomType> values;

		string emptyString;

		string& trim(string&& str) {
		    int g = -1;

		    while(str[++g] != '\0') {
		        if(str[g] != ' ') {
		            break;
		        }
		    }

			str.replace(0, g, emptyString);

			g = str.length();

		    while(g-- > 0) {
				if(str[g] != ' ') {
		            break;
		        }
		    }

			return str.replace(g+1, str.length(), emptyString);
		}

		double calcExp(vector<double>& numbers) {
			int j = 0;
			vector<double>::iterator it;
			double result = numbers[0];

			for (it=numbers.begin(); it!=numbers.end(); ++it) {
				// cout << "N: " << *it <<endl;
				// cout << "R: " << result <<endl;
				if( j % 2 != 0) {

					if(*it == 0) {
						result += (*(1+it));
					}

					if(*it == 1) {
						result -= (*(1+it));
					}

					if(*it == 2) {
						result *= (*(1+it));
					}

					if(*it == 3) {
						result /= (*(1+it));
					}

				}
				
				j++;
			}

			numbers.clear();

			return result;
		}

		unsigned int findEndTerm(string& str, unsigned int i) {
			unsigned short deep = 1;

			while(str[i] != '\0') {

				if( str[i] == '(' ) {
					deep++;
				}

				if( str[i] == ')' ) {
					deep--;
				}

				if(deep == 0)
					return i;

				i++;
			}

			return i;
		};

		double prepareValue(string& str, string& value, unsigned int& pos_start, unsigned int& i) {

			short pos_z = 0;

			short first_operator = 0; // (+) = 1  (-) = -1

			if(value[pos_z] == '-') {
				first_operator = -1;
			}
			if(first_operator == 0 && value[pos_z] == '+') {
				first_operator = 1;
			}
			if(first_operator != 0) {
				value = value.substr(1, value.length()-1);
			}

			if(value[pos_z] == '!') {
				while(value[pos_z] == '!' && ++pos_z);
				value = value.substr(pos_z, value.length());
			}

			// if(value == "true" || value == "+true") {
			// 	value = "1";
			// }
			// if(value == "-true") {
			// 	value = "-1";
			// }
			// if(value == "false" || value == "+false" || value == "-false") {
			// 	value = "0";
			// }

			if(value == "true") {
				value = "1";
			}
			if(value == "false") {
				value = "0";
			}
			if(!isdigit(value[0])) {

				size_t found = value.find("@");
				int num_loop = 0;
				int p1 = 0;

				if(found != string::npos) {
					p1 = found+1;

					found = value.find(".");

					num_loop = stoi(value.substr(p1, found - p1));

					value = values[loopData[num_loop]].getVector().at(loopPosition[num_loop])[value.substr(found+1, value.length() - found+1)];
				}
			}

			double d = atof(value.c_str());

			if(first_operator != 0) {
				d *= first_operator;
			}

			if(d <= 1 && pos_z > 0 && pos_z % 2 != 0) {
				d = !d;
			}
			if(d > 1 && pos_z > 0) {
				d = (pos_z % 2 == 0) ? 1 : 0;
			}

			value = to_string(d);

			str.replace(pos_start, i - pos_start, value);

			i -= (i - pos_start) - value.length();

			return d;
		};

		double parseArithmeticMD(string& str) {

			bool find_value = false;
			bool find_arith = false;
			bool prev_is_operator = true;

			double result = 0;

			unsigned int i = 0;
			unsigned int pos_arith = 0;
			unsigned int pos_start = 0;
			unsigned int pos_sym   = 0;
			vector<double> numbers;

			string lhs;
			string rhs;
			bool operationMD = false;

			/* 	0 - +
			    1 - -
			    2 - *
			    3 - /
			*/

			while(str[i] != '\0') {

				// Пропускаем пробелы
				if(isspace(str[i])) {
					i++;
					continue;
				}

				// Запоминаем стартовую позицию значения
				if(!find_value) {
					
					find_value = true;

					pos_start = i;

					if((str[i] == '+' || str[i] == '-') && prev_is_operator ) {
						bool b = true;

						pos_sym = i;

						while((str[i] == '-' || str[i] == '+' || isspace(str[i]))) {
							if(str[i] == '-') {
								b = !b;
							}
							i++;
						}

						if(b) {
							str.replace(pos_sym, i - pos_sym, "+");
						}
						else {
							str.replace(pos_sym, i - pos_sym, "-");
						}

						i = pos_sym;

						if(str[i+1] != '\0')
							i++;
					}

					prev_is_operator = false;

					continue;
				}

				// Определяем значение
				if(find_value) {

					if(!find_arith) {
						find_arith = true;
						pos_arith = pos_start;
					}

					if( str[i] == '+' || str[i] == '-' ) {

						prev_is_operator = true;
						find_value       = false;
						find_arith  = false;

						bool b = true;

						pos_sym = i;

						while((str[i] == '-' || str[i] == '+' || isspace(str[i]))) {
							if(str[i] == '-') {
								b = !b;
							}
							i++;
						}

						if(b) {
							str.replace(pos_sym, i - pos_sym, "+");
						}
						else {
							str.replace(pos_sym, i - pos_sym, "-");
							
						}

						i = pos_sym;

						rhs = trim(str.substr(pos_start, i - pos_start));

						numbers.push_back(prepareValue(str, rhs, pos_start, i));

						result = calcExp(numbers);

						lhs = to_string(result);

						str.replace(pos_arith, i - pos_arith, lhs);

						i = pos_arith + lhs.length() + 1;

						operationMD = false;

						continue;
					}
					
					if( str[i] == '*' || str[i] == '/' ) {

						find_value = false;
						find_arith = true;
						prev_is_operator = true;

						lhs = trim(str.substr(pos_start, i - pos_start));

						numbers.push_back(prepareValue(str, lhs, pos_start, i));

						pos_start = i;

						if(str[i] == '*') {
							numbers.push_back(2);
						}
						if(str[i] == '/') {
							numbers.push_back(3);
						}

						operationMD = true;

						i++;

						continue;
					}


					if( str[i+1] == '\0' ) {
						i++;

						rhs = trim(str.substr(pos_start, i - pos_start));

						numbers.push_back(prepareValue(str, rhs, pos_start, i));

						if(!operationMD) {
							break;
						}

						result = calcExp(numbers);

						str.replace(pos_arith, i - pos_arith, to_string(result));

						break;
					}
						

				}

				i++;
			}

			return result;
		};

		double parseArithmetic(string& str) {

			bool find_value = false;
			bool prev_is_operator = true;
			double result = 0;

			unsigned int i = 0;
			unsigned int pos_start = 0;
			unsigned int pos_sym = 0;
			vector<double> numbers;

			string lhs;
			string rhs;

			/* 	0 - +
			    1 - -
			    2 - *
			    3 - /
			*/

			result = parseArithmeticMD(str);

			while(str[i] != '\0') {

				// Пропускаем пробелы
				if(isspace(str[i])) {
					i++;
					continue;
				}

				// Запоминаем стартовую позицию значения
				if(!find_value) {
					
					find_value = true;

					pos_start = i;

					if( (str[i] == '+' || str[i] == '-') && prev_is_operator ) {
						i++;
					}

					prev_is_operator = false;

					continue;
				}

				// Определяем значение
				if(find_value) {

					if( str[i] == '+' || str[i] == '-' ) {

						prev_is_operator = true;

						find_value = false;

						bool b = true;

						pos_sym = i;

						while((str[i] == '-' || str[i] == '+' || isspace(str[i]))) {
							if(str[i] == '-') {
								b = !b;
							}
							i++;
						}

						if(b) {
							str.replace(pos_sym, i - pos_sym, "+");
						}
						else {
							str.replace(pos_sym, i - pos_sym, "-");
							
						}

						i = pos_sym;

						lhs = trim(str.substr(pos_start, i - pos_start));

						numbers.push_back(prepareValue(str, lhs, pos_start, i));

						if(str[i] == '+') {
							numbers.push_back(0);
						}
						if(str[i] == '-') {
							numbers.push_back(1);
						}

						i++;

						continue;
					}

					if( str[i+1] == '\0' ) {
						i++;

						rhs = trim(str.substr(pos_start, i - pos_start));

						numbers.push_back(prepareValue(str, rhs, pos_start, i));

						break;
					}
						

				}

				i++;
			}

			return calcExp(numbers);
		};

		double parseCompare(string& lhs, string& rhs, string& compare) {

			double Llhs = 0;
			double Lrhs = 0;

			if(lhs != "") {
				Llhs = parseArithmetic(lhs);
			}

			Lrhs = parseArithmetic(rhs);

			if(lhs != "") {
				if(compare == "==") {
					return Llhs == Lrhs ? 1 : 0;
				}
				if(compare == "!=") {
					return Llhs != Lrhs ? 1 : 0;
				}
				if(compare == "<=") {
					return Llhs <= Lrhs ? 1 : 0;
				}
				if(compare == ">=") {
					return Llhs >= Lrhs ? 1 : 0;
				}
				if(compare == "<") {
					return Llhs < Lrhs ? 1 : 0;
				}
				if(compare == ">") {
					return Llhs > Lrhs ? 1 : 0;
				}
			}

			return Lrhs;
		};

		void parseGL(string& str) {

			bool find_value = false;
			bool find_arith = false;
			bool first_logic = true;
			unsigned int pos_arith = 0;

			unsigned int i = 0;
			unsigned int pos_start = 0;

			string lhs;
			string rhs;
			string compare;
			string sub;

			/* 	1 - true
			    0 - false
			   -1 - &&
			   -2 - ||
			*/

			while(str[i] != '\0') {

				// Пропускаем пробелы
				if(isspace(str[i])) {
					i++;
					continue;
				}

				// Запоминаем стартовую позицию значения
				if(!find_value) {
					
					find_value = true;

					pos_start = i;

					continue;
				}

				// Определяем значение
				if(find_value) {

					if( str[i] == '&' || str[i] == '|' || (str[i] == '=' || str[i] == '!' && str[i+1] == '=') ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}
						
						find_value = false;
						find_arith = false;

						if(str[i] == '|' && first_logic) {
							rhs = trim(str.substr(pos_start, i - pos_start));

							if(parseCompare(lhs, rhs, compare) >= 1) {
								return;
							}
						}

						if(str[i] == '&')
							first_logic = false;

						if(compare == "<" || compare == "<=" || compare == ">" || compare == ">=") {

							rhs = trim(str.substr(pos_start, i - pos_start));

							sub = to_string(parseCompare(lhs, rhs, compare));

							str.replace(pos_arith, i - pos_arith, sub);

							i -= (i - pos_arith) - sub.length();
						}

						i += 2;

						compare = "==";

						lhs.assign(emptyString);

						continue;
					}

					
					if( str[i] == '>' || str[i] == '<' ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}

						find_value = false;

						if(!find_arith) {
							find_arith = true;

							pos_arith = pos_start;
						}

						sub = trim(str.substr(pos_start, i - pos_start));

						if(lhs != "") {
							if( parseCompare(lhs, sub, compare) ) {
								lhs = "1";
							} else {
								lhs = "0";
							}
						}
						else {
							lhs = sub;
						}

						pos_start = i;

						i++;

						if(str[i] == '=')
							i++;

						compare = str.substr(pos_start, i - pos_start);

						continue;
					}

					if( str[i] == '(' ) {

						// unsigned int term_start = i;
						unsigned int term_end = findEndTerm(str, i+1);

						double d = parseIF(trim(str.substr(i+1, term_end-1 - i)));

						if(d < 0 && str[i-1] == '-'){
							d *= -1;
							str.replace(i-1, term_end+2 - i, "+" + to_string(d));
						}
						else {
							str.replace(i, term_end+1 - i, to_string(d));
						}

					}


					if( str[i+1] == '\0' ) {

						if(compare == "<" || compare == "<=" || compare == ">" || compare == ">=") {

							rhs = trim(str.substr(pos_start, i+1 - pos_start));

							str.replace(pos_arith, i+1 - pos_arith, to_string(parseCompare(lhs, rhs, compare)));

						}

						return;
					}
						

				}

				i++;
			}
		};

		double parseIF(string& str) {

			parseGL(str);

			bool find_value = false;
			bool first_logic = true;

			unsigned int i = 0;
			
			unsigned int pos_start = 0;
			vector<double> bits;
			string lhs;
			string rhs;
			string compare;

			/* 	1 - true
			    0 - false
			   -1 - &&
			   -2 - ||
			*/

			while(str[i] != '\0') {

				// Пропускаем пробелы
				if(isspace(str[i])) {
					i++;
					continue;
				}

				// Запоминаем стартовую позицию значения
				if(!find_value) {
					
					find_value = true;

					pos_start = i;

					continue;
				}

				// Определяем значение
				if(find_value) {

					if( (str[i] == '&' && str[i+1] == '&') || (str[i] == '|' && str[i+1] == '|') ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}

						find_value = false;

						rhs = trim(str.substr(pos_start, i - pos_start));

						pos_start = i;

						double d = parseCompare(lhs, rhs, compare);

						bits.push_back(d);

						// lhs = "";
						lhs.assign(emptyString);

						if(str[i] == '&') {

							bits.push_back(-1);

						}
						else {

							if(first_logic && d >= 1) {
								return 1;
							}

							bits.push_back(-2);

						}

						i += 2;

						first_logic = false;

						continue;
					}

					if( str[i] == '=' || str[i] == '!' && str[i+1] == '=' ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}

						find_value = false;

						string s = trim(str.substr(pos_start, i - pos_start));

						if(lhs != "") {
							lhs = parseCompare(lhs, s, compare) ? "1" : "0";
						}
						else {
							lhs = s;
						}

						pos_start = i;

						i += 2;

						// if(str[i] == '=')
						// 	i++;

						compare = str.substr(pos_start, i - pos_start);

						continue;
					}

					if( str[i+1] == '\0' ) {

						rhs = trim(str.substr(pos_start, i+1 - pos_start));

						bits.push_back(parseCompare(lhs, rhs, compare));

					}

				}

				i++;
			}

			if(bits.size() == 0 && bits.size() % 2 == 0 )
				return 0;

			if(bits.size() == 1)
				return bits[0];


			i = 0;
			double result = bits[0];

			unsigned int vc_sz = bits.size();

			for (i = 0; i < vc_sz; i++) {

				if( i % 2 != 0) {

					if(bits.at(i) == -1) {
						result = result && bits.at(i+1);
					}

					if(bits.at(i) == -2) {
						result = result || bits.at(i+1);
					}

				}
			}

			return result;
		};

		double parseIF(string&& str) {

			parseGL(str);

			bool find_value = false;
			bool first_logic = true;

			unsigned int i = 0;
			
			unsigned int pos_start = 0;
			vector<double> bits;
			string lhs;
			string rhs;
			string compare;

			/* 	1 - true
			    0 - false
			   -1 - &&
			   -2 - ||
			*/

			while(str[i] != '\0') {

				// Пропускаем пробелы
				if(isspace(str[i])) {
					i++;
					continue;
				}

				// Запоминаем стартовую позицию значения
				if(!find_value) {
					
					find_value = true;

					pos_start = i;

					continue;
				}

				// Определяем значение
				if(find_value) {

					if( (str[i] == '&' && str[i+1] == '&') || (str[i] == '|' && str[i+1] == '|') ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}

						find_value = false;

						rhs = trim(str.substr(pos_start, i - pos_start));

						pos_start = i;

						double d = parseCompare(lhs, rhs, compare);

						bits.push_back(d);

						// lhs = "";
						lhs.assign(emptyString);

						if(str[i] == '&') {

							bits.push_back(-1);

						}
						else {

							if(first_logic && d >= 1) {
								return 1;
							}

							bits.push_back(-2);

						}

						i += 2;

						first_logic = false;

						continue;
					}

					if( str[i] == '=' || str[i] == '!' && str[i+1] == '=' ) {

						if(i - pos_start == 0) {
							i++;
							continue;
						}

						find_value = false;

						string s = trim(str.substr(pos_start, i - pos_start));

						if(lhs != "") {
							lhs = parseCompare(lhs, s, compare) ? "1" : "0";
						}
						else {
							lhs = s;
						}


						pos_start = i;

						i += 2;

						// if(str[i] == '=')
						// 	i++;

						compare = str.substr(pos_start, i - pos_start);


						continue;
					}

					if( str[i+1] == '\0' ) {

						rhs = trim(str.substr(pos_start, i+1 - pos_start));

						bits.push_back(parseCompare(lhs, rhs, compare));

					}

				}

				i++;
			}

			if(bits.size() == 0 && bits.size() % 2 == 0 )
				return 0;

			if(bits.size() == 1)
				return bits[0];


			i = 0;
			double result = bits[0];

			unsigned int vc_sz = bits.size();

			for (i = 0; i < vc_sz; i++) {

				if( i % 2 != 0) {

					if(bits.at(i) == -1) {
						result = result && bits.at(i+1);
					}

					if(bits.at(i) == -2) {
						result = result || bits.at(i+1);
					}

				}
			}

			return result;
		};

		void parseVar(string& str, unsigned int& i, unsigned int& pos_start, map<string, unsigned short>& points) {
			unsigned int pos_end = i+2;
			map<string, unsigned short>::iterator points_it;
			int range = pos_end - pos_start;
			string str_value;
			unsigned int str_value_length = 0;

			// получаем имя плейсхолдера
			string val(trim( str.substr(pos_start+2, range - 4) ));

			size_t found = val.find("~");
			if(found != string::npos) {
				val.assign(trim(val.substr(found+1, val.length())));

				found = val.find("@");
				int num_loop = 0;
				int p1 = 0;

				if(found != string::npos) {
					p1 = found+1;

					found = val.find(".");

					num_loop = stoi(val.substr(p1, found - p1));

					str_value.assign(values[loopData[num_loop]].getVector().at(loopPosition[num_loop])[val.substr(found+1, val.length() - found+1)]);
					str_value_length = str_value.length();
				}
				else {

					gl_values_it = values.find(val);

					if(gl_values_it != values.end()) {
						str_value.assign(gl_values_it->second.getString());
						// str_value = gl_values_it->second.getString();
						str_value_length = str_value.length();
					}
				}

				// пересчитываем длину значения плейсхолдера
				for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
					points_it->second -= range;
					points_it->second += str_value_length;
				}

				// str = str.replace(pos_start, range, str_value);
				str.replace(pos_start, range, str_value);

				i = pos_start + str_value_length;

				return;
			}

			found = val.find("@");
			int num_loop = 0;
			int p1 = 0;

			if(found != string::npos) {
				p1 = found+1;

				found = val.find(".");

				num_loop = stoi(val.substr(p1, found - p1));

				str_value.assign(values[loopData[num_loop]].getVector().at(loopPosition[num_loop])[val.substr(found+1, val.length() - found+1)]);
				str_value_length = str_value.length();
			}
			else {

				gl_values_it = values.find(val);

				if(gl_values_it != values.end()) {
					str_value.assign(gl_values_it->second.getString());
					// str_value = gl_values_it->second.getString();
					str_value_length = str_value.length();
				}
			}

			points_it = points.find(val);

			if(points_it == points.end()) {
				// удаляем вышедшие за пределы плейсхолдеры
				for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
					if(i >= points_it->second) {
						// cout << "delete-1 " << points_it->first << endl;
						points.erase(points_it);
					}
				}

				// пересчитываем длину значения плейсхолдера
				for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
					points_it->second -= range;
					points_it->second += str_value_length;
				}

				points.emplace(val, pos_start + str_value_length);
			}
			else {
				// если плейсхолдер с таким названием существует и находится в пределах
				// плейсхолдера с таким же названием, то вырезаем его.
				if(points_it != points.end() && i < points_it->second) {
					// cout << "cut " << points_it->first << endl;
					
					// str = str.replace(pos_start, range, "");
					str.replace(pos_start, range, emptyString);

					i = pos_start;

					for(points_it=points.begin(); points_it!=points.end(); ++points_it) {
						if(points_it->second >= range)
							points_it->second -= range;
					}

					return;
				}

				// удаляем вышедшие за пределы плейсхолдеры
				for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
					if(i >= points_it->second) {
						// cout << "delete-2 " << points_it->first << endl;
						points.erase(points_it);
					}
				}

				i = pos_start;

				return;
			}

			// str = str.replace(pos_start, range, str_value);
			str.replace(pos_start, range, str_value);

			i = pos_start;
		};

		unsigned int findEndFor(string& str, unsigned int& i, bool&& find_start_placeholder = true) {
			unsigned int pos_start = 0;
			unsigned int pos_end = 0;
			bool open_condition = false;
			short deep = 1;
			unsigned int start = i;

			while(str[i++] != '\0') {

				if(str[i] == '{' && str[i+1] == '%') {
					open_condition = true;
					pos_start = i;
					continue;
				}
				if(open_condition) {
					if(str[i] == '%' && str[i+1] == '}') {
						pos_end = i+2;

						// получаем имя плейсхолдера
						string val = trim(str.substr(pos_start+2, pos_end - pos_start - 4) );

						if(deep > 0 && val == "endfor") {
							deep--;
							open_condition = false;

							if(deep == 0) {

								if(find_start_placeholder) {
									i = pos_start - 1;
									return pos_start;
								}

								return pos_end;
							}

							continue;
						}

						size_t found = val.find("for");
						if(found != string::npos) {
							deep++;
							open_condition = false;
						}
					}
				}
			}

			if(deep > 0 && str.length() == i - 1) {
				i = start;
				return 0;
			}

			return pos_start;
		};

		unsigned int findEndFor2(string& str, unsigned int& i) {
			unsigned int pos_start = 0;
			unsigned int pos_end = 0;
			bool open_condition = false;
			short deep = 1;
			unsigned int start = i;

			while(str[i++] != '\0') {

				if(str[i] == '{' && str[i+1] == '%') {
					open_condition = true;
					pos_start = i;
					continue;
				}
				if(open_condition) {
					if(str[i] == '%' && str[i+1] == '}') {
						pos_end = i+2;

						// получаем имя плейсхолдера
						string val = trim(str.substr(pos_start+2, pos_end - pos_start - 4) );

						if(deep > 0 && val == "endfor") {
							deep--;
							open_condition = false;

							if(deep == 0) {

								i += 2;

								break;
							}

							continue;
						}

						size_t found = val.find("for");
						if(found != string::npos) {
							deep++;
							open_condition = false;
						}
					}
				}
			}

			if(deep > 0 && str.length() == i - 1) {
				i = start;
				return 0;
			}

			return pos_start;
		};

		unsigned int findEndIf(string& str, unsigned int& i, bool&& find_start_placeholder = true) {
			unsigned int pos_start = 0;
			unsigned int pos_end = 0;
			bool open_condition = false;
			short deep = 1;
			unsigned int start = i;

			while(str[i++] != '\0') {

				if(str[i] == '{' && str[i+1] == '%') {
					open_condition = true;
					pos_start = i;
					continue;
				}
				if(open_condition) {
					if(str[i] == '%' && str[i+1] == '}') {
						pos_end = i+2;

						// получаем имя плейсхолдера
						string val = trim(str.substr(pos_start+2, pos_end - pos_start - 4) );

						if(deep > 0 && val == "endif") {
							deep--;
							open_condition = false;

							if(deep == 0) {

								if(find_start_placeholder) {
									i = pos_start - 1;
									return pos_start;
								}

								return pos_end;
							}

							continue;
						}

						size_t found = val.find("if ");
						if(found != string::npos) {
							deep++;
							open_condition = false;
						}
					}
				}
			}

			if(deep > 0 && str.length() == i - 1) {
				i = start;
				return 0;
			}

			return pos_start;
		};

		int foundStr(string& str, string&& val, unsigned int pos_start = 0, unsigned int pos_end = 0) {
			unsigned int offset_start = pos_start;
			unsigned int p = pos_start;
			unsigned int pos_symbol = 0;

			if(pos_end == 0)
				pos_end = str.length();

			while(p < pos_end) {
				if(str[p] == val[pos_symbol]) {

					// cout << str[p] << endl;

					if(pos_symbol == 0)
						offset_start = p;

					if(pos_symbol+1 == val.length()) {
						// cout << offset_start << endl;
						return offset_start;
					}

					pos_symbol++;
				}
				else {
					pos_symbol = 0;
				}
				p++;
			}

			return -1;
		}

		void replaceAllPlaceholders(string& str, const string& from, const string& to) {

			bool open_var = false;
		    bool open_condition = false;
		    unsigned int i = 0;
		    unsigned int pos_start = 0;
		    // map<string, CustomType>::iterator values_it;

		    while( str[i] != '\0' ) {
				if(str[i] == '{') {
					if(str[i+1] == '{') {
						open_var = true;
						pos_start = i;
						i++;
						continue;
					}
					if(str[i+1] == '%') {
						open_condition = true;
						pos_start = i;
						i++;
						continue;
					}
				}

				if(open_var || open_condition) {
					if(str[i+1] == '}') {
						if(str[i] == '}') {
							open_var = false;

							string val = str.substr(pos_start, i+2-pos_start);

							size_t found = val.find(from+".");
							if(found != string::npos) {

								str.replace(found+pos_start, from.length(), to);

								i += 2 + to.length() - from.length();
							}

							continue;
						}
						if(str[i] == '%') {
							open_condition = false;

							i += 2;

							string val = trim(str.substr(pos_start+2, i-pos_start - 4) );

							if(val == "endfor" || val == "endif")
								continue;

							unsigned int pos_i = i;
							unsigned int pos_start_condition = 0;


							size_t found = val.find("for");
							if(found != string::npos) {
								pos_start_condition = findEndFor2(str, i);
							}
							found = val.find("if ");
							if(found != string::npos) {


								unsigned int offset_start = pos_start+2;
								unsigned int p = pos_start+2;
								unsigned int pos_symbol = 0;

								val = from+".";

								while(p < i-2) {
									if(str[p] == val[pos_symbol]) {

										if(pos_symbol == 0)
											offset_start = p;

										if(pos_symbol+1 == val.length()) {
											str.replace(offset_start, val.length(), to+".");
											p = offset_start + to.length();
											i += to.length() - from.length();
										}

										pos_symbol++;
									}
									else {
										pos_symbol = 0;
									}
									p++;
								}

								continue;
							}
							
							if(pos_start_condition == 0) {
								return;
							}

							string substr = str.substr(pos_start, i-pos_start);

							parseConditionOnlyReplacePlaceholders(substr, pos_i-2-pos_start, 0);

							i = pos_i;

							str.replace(i, (pos_start_condition-i), substr);

							continue;
						}
					}
				}

		    	i++;
		    }

		}

		void parseConditionOnlyReplacePlaceholders(string& str, unsigned int&& i, unsigned int&& pos_start) {
			string substr;
			unsigned int pos_end = i+2;

			// получаем имя плейсхолдера
			string val = trim(str.substr(pos_start+2, pos_end - pos_start - 4) );

			size_t found = val.find("for");
			if(found != string::npos) {

				unsigned int pos_endfor = findEndFor(str, i, true);

				if(pos_endfor == 0) {
					return;
				}

				substr = str.substr(pos_end, pos_endfor - pos_end);


				str.replace(pos_start, pos_end + substr.length() - pos_start, emptyString);
				i -= pos_end + substr.length() - pos_start - 2;


				string for_item;

				size_t found_in = val.find(" in ");
				if(found_in != string::npos) {
					for_item = trim(val.substr(found+3, found_in-3));
				}

				for_deep++;

				string to = "@" + to_string(for_deep);

				replaceAllPlaceholders(substr, for_item, to);

				str = substr;	
				
				for_deep--;

				return;
			}

		};

		void parseCondition(string& str, unsigned int& i, unsigned int& placeholder_for_start, map<string, unsigned short>& points) {
			unsigned int placeholder_for_end = i+2;
			map<string, unsigned short>::iterator points_it;

			// получаем содержимое плейсхолдера
			string val(trim(str.substr(placeholder_for_start+2, placeholder_for_end - placeholder_for_start - 4) ));

			// удаляем вышедшие за пределы плейсхолдеры
			for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
				if(i >= points_it->second) {
					points.erase(points_it);
				}
			}

			size_t found = val.find("for");
			if(found != string::npos) {

				unsigned int placeholder_endfor_start = findEndFor(str, i, true);
				unsigned int placeholder_endfor_end   = findEndFor(str, i, false);

				string substr(str.substr(placeholder_for_end, placeholder_endfor_start - placeholder_for_end));

				if(placeholder_endfor_start == 0) {
					return;
				}

				// str = str.replace(placeholder_for_start, placeholder_endfor_end - placeholder_for_start, "");
				str.replace(placeholder_for_start, placeholder_endfor_end - placeholder_for_start, emptyString);

				i = placeholder_for_start;

				string for_item;
				string for_items;

				size_t found_in = val.find(" in ");
				if(found_in != string::npos) {
					for_item.assign(trim(val.substr(found+3, found_in-3)));

					for_items.assign(trim(val.substr(found_in+4, val.length())));
				}

				for_deep++;

				loopData[for_deep] = for_items;

				string to = "@" + to_string(for_deep);

				replaceAllPlaceholders(substr, for_item, to);

				// пересчитываем длину значения плейсхолдера
				for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
					if(points_it->second >= (placeholder_endfor_end - placeholder_for_start))
						points_it->second -= placeholder_endfor_end - placeholder_for_start;
				}

				vector< map<string, string> > vc = values[for_items].getVector();

				string ss;

				// map<string, string>::iterator m_it;
				map<string, unsigned short> _points;
				// map<string, CustomType>::iterator values_it_find;
				unsigned int for_pos = 0;
				int sz_vc = vc.size();
				int sz_point = substr.length();

				for (for_pos = 0; for_pos < sz_vc; for_pos++) {

					// cout << (*vc_it)["id"] << endl;

					loopPosition[for_deep] = for_pos;

					ss.assign(substr);

					for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
						_points.emplace(points_it->first, sz_point);
					}

				    parse(ss, _points);

				    str.replace(i, 0, ss);

					i += ss.length();

					for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
						points_it->second += _points.find(points_it->first)->second;
					}

					_points.clear();
				}
				
				for_deep--;

				return;

			}

			else {
			

				if(parseIF(move(val.substr(found+3, val.length()-found-3)))) {
					unsigned int pos_end_offset = findEndIf(str, i, true);
					
					if(pos_end_offset == 0) {
						return;
					}

					unsigned int pos_end_global = findEndIf(str, i, false);

					str.replace(placeholder_for_start, pos_end_global - placeholder_for_start, str.substr(placeholder_for_end, pos_end_offset - placeholder_for_end));

					// пересчитываем длину значения плейсхолдера
					for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
						points_it->second -= pos_end_global - placeholder_for_start;
						points_it->second += pos_end_offset - placeholder_for_end;
					}
				}
				else {
					unsigned int pos_end_global = findEndIf(str, i, false);

					str.replace(placeholder_for_start, pos_end_global - placeholder_for_start, emptyString);

					// пересчитываем длину значения плейсхолдера
					for (points_it=points.begin(); points_it!=points.end(); ++points_it) {
						points_it->second -= pos_end_global - placeholder_for_start;
					}
				}

				i = placeholder_for_start;

				return;

			}
		};


	public:

		TemplateEngine(map<string, CustomType>& _values) {
			for_deep = 0;
			if_deep = 0;
			values = _values;
		}

		void parse(string& str) {
			map<string, unsigned short> points;

			parse(str, points);
		}

		void parse(string& str, map<string, unsigned short>& points) {
			unsigned int i = 0;
			unsigned int pos_start = 0;
			bool open_var = false;
			bool open_condition = false;

			while(str[i] != '\0') {

				if(str[i] == '{') {
					if(str[i+1] == '{') {
						open_var = true;
						pos_start = i;
						i++;
						continue;
					}
					if(str[i+1] == '%') {
						open_condition = true;
						pos_start = i;
						i++;
						continue;
					}
				}

				if(open_var || open_condition) {
					if(str[i+1] == '}') {
						if(str[i] == '}') {
							open_var = false;
							parseVar(str, i, pos_start, points);
							continue;
						}
						if(str[i] == '%') {
							open_condition = false;
							parseCondition(str, i, pos_start, points);
							continue;
						}
					}
				}
				
				i++;
			}
		};

};


int main(int argc, char* argv[]) {

	vector< map<string, string> > vc = {
		map<string, string>({
			{"id","1"},
			{"name","Alex .{{loop}}."}
		})
		,map<string, string>({
			{"id","2"},
			{"name","Bob"}
		})
		,map<string, string>({
			{"id","3"},
			{"name","BJ"}
		})
		,map<string, string>({
			{"id","3"},
			{"name","Jack"},
			{"login","BJ"}
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","2"},
			{"name","Bob"}
		})
		,map<string, string>({
			{"id","3"},
			{"name","Jack}"},
			{"login","BJ"}
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","2"},
			{"name","Bob"}
		})
		,map<string, string>({
			{"id","3"},
			{"name","Jack"},
			{"login","BJ"}
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","3"},
			{"name","Jack}"},
			{"login","BJ"}
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","2"},
			{"name","Bob"}
		})
		,map<string, string>({
			{"id","3"},
			{"name","Jack"},
			{"login","BJ"}
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
		,map<string, string>({
			{"id","4"},
			{"name","Marco"},
		})
	};

	string asd = ""

		"{% for item in items %}"

			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"
			"{{~item.name }}"

			"{%if item.id <= 20%}qweqweqweqw {{~item.name }} qeweqweqw qweqeqweq{%endif%}"
			"{%if item.id <= 20%}qweqweqweqw {{~item.name }} qeweqweqw qweqeqweq{%endif%}"

		"{% endfor %}"

	;

	string asd2 = 
		"{% for item in items %}"

			"-> {{~ item.id }} - {{~ item.name }}\n"

		"{%endfor%}"
	;


	map<string, CustomType> values = {

		{"head",         CustomType("h {{loop}} h")},
		{"data",         CustomType("d {{head}} d")},
		{"body",         CustomType(" b {{header}}{{mobile_aside}}{{footer}} b ")},
		{"header",       CustomType(" h {{data}}{{mobile_aside}} h ")},
		{"footer",       CustomType(" f {{body}} f ")},
		{"mobile_aside", CustomType(" a {{header}}{{menu}}{{footer}}{{test}} a ")},
		{"menu",         CustomType("menu")},
		{"test",         CustomType("head {{data}}")},
		{"bbb",          CustomType("3")},
		{"text",         CustomType("2")},
		{"item",         CustomType("{{head}}")},
		{"w",            CustomType(".{{~item}}.")},
		{"@123.name",    CustomType("allo")},
		{"loop",         CustomType(asd)},
		{"loop2",        CustomType(asd2)},

		{"items",        CustomType(vc)}

	};

	// test["string"].getString();
	// test["int"].getInt();
	// test["char"].getChar();
	// test["double"].getDouble();
	// test["vector"].getVector()[1]["name"];

	TemplateEngine *t = new TemplateEngine(values);

	string str = " {{ loop }}";

	if(argv[1])
		str.assign(argv[1]);

	t->parse(str);

	cout << str << endl;

	return 0;
}