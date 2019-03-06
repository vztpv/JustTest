
		private:
			static int FindRight(VECTOR<Token2> & strVec, int start, int last, const wiz::LoadDataOption & option)
			{
				for (int i = last; i >= start; --i) {
					if (strVec[i].len == 1 && -1 != Equal(option.Right, strVec[i].str[0])) {
						return i;
					}
				}

				return -1;
			}

			static void Merge(UserType * next, UserType * ut, UserType * *ut_next)
			{
				//check!!
				while (ut->GetIListSize() >= 1 && ut->GetUserTypeListSize() >= 1
					&& (ut->GetUserTypeList(0)->GetName() == "##" ||
						ut->GetUserTypeList(0)->GetName() == "#"))
				{
					ut = ut->GetUserTypeList(0);
				}

				//int chk = 0;
				while (true) {
					int itCount = 0;
					int utCount = 0;

					UserType* _ut = ut;
					UserType* _next = next;

					if (ut_next && _ut == *ut_next) {
						*ut_next = _next;
					}

					for (int i = 0; i < _ut->GetIListSize(); ++i) {
						if (_ut->IsUserTypeList(i)) {
							if (_ut->GetUserTypeList(utCount)->GetName() == "#" ||
								_ut->GetUserTypeList(utCount)->GetName() == "##") {
								_ut->GetUserTypeList(utCount)->SetName("");
							}
							else {
								{
									_next->LinkUserType(_ut->GetUserTypeList(utCount));
									_ut->GetUserTypeList(utCount) = nullptr;
								}
							}
							utCount++;
						}
						else if (_ut->IsItemList(i)) {
							_next->AddItemList(std::move(_ut->GetItemList(itCount)));
							itCount++;
						}
					}
					_ut->Remove();
					if (ut->GetParent() && next->GetParent()) {
						ut = ut->GetParent();
						next = next->GetParent();
					}
					else {
						break;
					}
				}

			}

		public:
			static bool __LoadData5(VECTOR<Token2> * _strVec, int start_idx, int last_idx, UserType * _global, const wiz::LoadDataOption * _option,
				int start_state, int last_state, UserType * *next) // first, strVec.empty() must be true!!
			{
				std::vector<std::string> varVec;
				std::vector<std::string> valVec;

				if (start_idx > last_idx) {
					return false;
				}

				VECTOR<Token2>& strVec = *_strVec;
				UserType& global = *_global;
				const wiz::LoadDataOption& option = *_option;

				int state = start_state;
				int braceNum = 0;
				std::vector< UserType* > nestedUT(1);
				std::string var, val;


				nestedUT[0] = &global;

				int i = start_idx;

				while (false == strVec.empty() && i <= last_idx) {
					switch (state)
					{
					case 0:
					{
						if (strVec[i].len == 1 && -1 != Equal(option.Left, strVec[i].str[0])) {
							i += 1;

							nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
							nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

							for (int i = 0; i < varVec.size(); ++i) {
								nestedUT[braceNum]->AddItem(std::move(varVec[i]), std::move(valVec[i]));
							}
							varVec.clear();
							valVec.clear();


							UserType temp("");

							nestedUT[braceNum]->AddUserTypeItem(temp);
							UserType* pTemp = nullptr;
							nestedUT[braceNum]->GetLastUserTypeItemRef("", pTemp);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
								nestedUT.push_back(nullptr);
							}

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp;
							///

							state = 0;
						}
						else if (strVec[i].len == 1 && -1 != Equal(option.Right, strVec[i].str[0])) {
							i += 1;

							state = 0;

							// UserTypeListsize?
							nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
							nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

							for (int i = 0; i < varVec.size(); ++i) {
								nestedUT[braceNum]->AddItem(std::move(varVec[i]), std::move(valVec[i]));
							}
							varVec.clear();
							valVec.clear();

							if (braceNum == 0) {
								UserType ut;
								ut.AddUserTypeItem(UserType("#")); // json -> "var_name" = val  // clautext, # is line comment delimiter.
								UserType* pTemp = nullptr;
								ut.GetLastUserTypeItemRef("#", pTemp);
								int utCount = 0;
								int itCount = 0;
								auto max = nestedUT[braceNum]->GetIListSize();
								for (auto i = 0; i < max; ++i) {
									if (nestedUT[braceNum]->IsUserTypeList(i)) {
										ut.GetUserTypeList(0)->AddUserTypeItem(std::move(*(nestedUT[braceNum]->GetUserTypeList(utCount))));
										utCount++;
									}
									else {
										ut.GetUserTypeList(0)->AddItemList(std::move(nestedUT[braceNum]->GetItemList(itCount)));
										itCount++;
									}
								}

								nestedUT[braceNum]->Remove();
								nestedUT[braceNum]->AddUserTypeItem(std::move(*(ut.GetUserTypeList(0))));

								braceNum++;
							}

							{
								if (braceNum < nestedUT.size()) {
									nestedUT[braceNum] = nullptr;
								}
								braceNum--;
							}
						}
						else {
							std::pair<bool, Token2> bsPair;

							if (i < last_idx)
							{
								bsPair = std::make_pair(true, strVec[i + 1]);
							}
							else {
								bsPair = std::make_pair(false, Token2());
							}

							if (bsPair.first) {
								if (bsPair.second.len == 1 && -1 != Equal(option.Assignment, bsPair.second.str[0])) {
									// var2
									var = std::string(strVec[i].str, strVec[i].len);
									state = 1;
									i += 1;
									i += 1;
								}
								else {
									// var1
									if (i <= last_idx) {
										val = std::string(strVec[i].str, strVec[i].len);

										varVec.push_back(var);
										valVec.push_back(val);
										//nestedUT[braceNum]->AddItem("", ""); // std::move(val));
										val = "";

										state = 0;
										i += 1;
									}
								}
							}
							else
							{
								// var1
								if (i <= last_idx)
								{
									val = std::string(strVec[i].str, strVec[i].len);
									varVec.push_back(var);
									valVec.push_back(val);
									//nestedUT[braceNum]->AddItem("", "");// std::move(val));
									val = "";

									state = 0;
									i += 1;
								}
							}
						}
					}
					break;
					case 1:
					{
						if (strVec[i].len == 1 && -1 != Equal(option.Left, strVec[i].str[0])) {
							i += 1;
							nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
							nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

							for (int i = 0; i < varVec.size(); ++i) {
								nestedUT[braceNum]->AddItem(std::move(varVec[i]), std::move(valVec[i]));
							}
							varVec.clear();
							valVec.clear();


							///
							{
								nestedUT[braceNum]->AddUserTypeItem(UserType(var));
								UserType* pTemp = nullptr;
								nestedUT[braceNum]->GetLastUserTypeItemRef(var, pTemp);
								var = "";
								braceNum++;

								/// new nestedUT
								if (nestedUT.size() == braceNum) {
									nestedUT.push_back(nullptr);
								}

								/// initial new nestedUT.
								nestedUT[braceNum] = pTemp;
							}
							///
							state = 0;
						}
						else {
							if (i <= last_idx) {
								val = std::string(strVec[i].str, strVec[i].len);

								i += 1;
								varVec.push_back(var);
								valVec.push_back(val);
								//nestedUT[braceNum]->AddItem("", ""); // std::move(var), std::move(val));
								var = ""; val = "";

								state = 0;
							}
						}
					}
					break;
					default:
						// syntax err!!
						throw "syntax error ";
						break;
					}
				}

				if (next) {
					*next = nestedUT[braceNum];
				}

				if (varVec.empty() == false) {
					nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
					nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

					for (int i = 0; i < varVec.size(); ++i) {
						nestedUT[braceNum]->AddItem(varVec[i], valVec[i]);
					}
					varVec.clear();
					valVec.clear();
				}

				if (state != last_state) {
					throw std::string("error final state is not last_state!  : ") + toStr(state);
				}
				if (i > last_idx + 1) {
					throw std::string("error i > last_idx + 1: " + toStr(i) + " , " + toStr(last_idx));
				}
				return true;
			}
			
			class Node { // all node has first
			public:
				wiz::Token2 var;
				wiz::Token2 val;
				Node* next = nullptr; //
				Node* first = nullptr; //
				Node* last = nullptr;  // only first node has parent, last.
				Node* parent = nullptr; // only first node has parent, last.
				Node* first_child = nullptr;
				bool is_ut = true; // false -> itemtype, true -> usertype.
				bool is_start = true;
				int size = 0;
				int ut_size = 0;
			};
			
			static Node* NewNode() {
				return new Node();
			}
			static Node* Parent(Node* x) {
				if (x && x->first) {
					return x->first->parent;
				}
				return nullptr;
			}

			static bool AddItem(Node* ut, wiz::Token2 var, wiz::Token2 val) {
				if (!ut || !ut->is_ut) {
					return false;
				}
				if (!ut->first_child) {
					Node* temp = NewNode();
					temp->var = var;
					temp->val = val;
					temp->parent = ut;
					temp->first = temp;
					temp->last = temp;
					temp->is_ut = false;
					temp->is_start = true;
					ut->first_child = temp;
					ut->first_child->last = temp;
				}
				else {
					Node* temp = NewNode();
					temp->var = var;
					temp->val = val;
					//
					temp->first = ut->first_child;
					temp->is_ut = false;
					temp->is_start = false;
					ut->first_child->last->next = temp;
					ut->first_child->last = temp;
				}
				ut->size++;
				return true;
			}
			static bool AddItem(Node* ut, Node* it) {
				if (!ut || !ut->is_ut) {
					return false;
				}
				it->next = nullptr;
				if (!ut->first_child) {
					Node* temp = it;
					temp->parent = ut;
					temp->first = temp;
					temp->last = temp;
					temp->is_ut = false;
					temp->is_start = true;
					ut->first_child = temp;
					ut->first_child->last = temp;
				}
				else {
					Node* temp = it;
					//
					temp->first = ut->first_child;
					temp->is_ut = false;
					temp->is_start = false;
					ut->first_child->last->next = temp;
					ut->first_child->last = temp;
				}
				ut->size++;
				return true;
			}
			static bool AddUserType(Node* ut, wiz::Token2 var) {
				if (!ut || !ut->is_ut) {
					return false;
				}
				if (!ut->first_child) {
					Node* temp = NewNode();
					temp->var = var;
					temp->parent = ut;
					temp->first = temp;
					temp->last = temp;
					temp->is_ut = true;
					temp->is_start = true;
					ut->first_child = temp;
					ut->first_child->last = temp;
				}
				else {
					Node* temp = NewNode();
					temp->var = var;
					//
					temp->first = ut->first_child;
					temp->is_ut = true;
					temp->is_start = false;
					ut->first_child->last->next = temp;
					ut->first_child->last = temp;
				}
				ut->size++;
				ut->ut_size++;
				return true;
			}
			static bool AddUserType(Node* ut, Node* other_ut) {
				if (!ut || !ut->is_ut || !other_ut->is_ut) {
					return false;
				}
				other_ut->next = nullptr;
				
				if (!ut->first_child) {
					other_ut->first = other_ut;
					ut->first_child = other_ut;
					ut->first_child->last = other_ut;
				}
				else {
					other_ut->first = ut->first_child;
					ut->first_child->last->next = other_ut;
					ut->first_child->last = other_ut;
				}
				ut->size++;
				ut->ut_size++;
				other_ut->parent = ut;
				other_ut->next = nullptr;
				return true;
			}

			static void _RemoveAll(Node* node) {
				if (node == nullptr) { return; }
				_RemoveAll(node->first_child);
				_RemoveAll(node->next);
				delete node;
			}
			static void RemoveAll(Node* node) { // node inner remove all
				if (node) {
					_RemoveAll(node->first_child);
					node->size = 0;
					node->ut_size = 0;
					node->first_child = nullptr;
				}
			}
			static void Clear(Node* node) {
				if (node) {
					node->size = 0;
					node->ut_size = 0;
					node->first_child = nullptr;
					node->first = nullptr;
					node->last = nullptr;
					node->next = nullptr;
				}
			}

			static void Print(Node* node, std::ostream& out) {
				if (nullptr == node) {
					return;
				}
				if (node->is_ut) {
					if (node->var.len == 0) {
						out << " { \n";
					}
					else {
						out << std::string(node->var.str, node->var.len) << " = { \n";
					}
				}
				else {
					if (node->var.len == 0) {
						out << std::string(node->val.str, node->val.len) << " ";
					}
					else {
						out << std::string(node->var.str, node->var.len) << " = ";
						out << std::string(node->val.str, node->val.len) << "\n";
					}
				}
				Print(node->first_child, out);
				if (node->is_ut) {
					out << " }\n";
				}
				Print(node->next, out);
			}
			static void Merge2(Node* next, Node* ut, Node** ut_next)
			{
				static Token2 EmptyName;
				EmptyName.str = nullptr;
				EmptyName.len = 0;
				//check!!
				while (ut->size > 0 && ut->ut_size > 0//ut->GetIListSize() >= 1 && ut->GetUserTypeListSize() >= 1
					&& ( ut->first_child->var == "##" || //ut->GetUserTypeList(0)->GetName() == "##" ||
						ut->first_child->var == "#" )) //ut->GetUserTypeList(0)->GetName() == "#"))
				{
					ut = ut->first_child;
				}

				//int chk = 0;
				while (true) {
					Node* _ut = ut;
					Node* _next = next;

					if (ut_next && _ut == *ut_next) {
						*ut_next = _next;
					}

					Node* iter = _ut->first_child;
					if (iter) {
						Node* last = iter->last;
						Node* next_iter = nullptr;

						for (; iter != nullptr; iter = next_iter) {
							next_iter = iter->next;
							if (iter->is_ut) {
								if (iter->var == "#" ||
									iter->var == "##") {
									iter->var = EmptyName;
								}
								else {
									{
										AddUserType(_next, iter);
										//LinkUserType(_next, iter);
									}
								}
							}
							else {
								AddItem(_next, iter);
								//_next->AddItemList(std::move(_ut->GetItemList(itCount)));
							}
						}
					}
					
					//_ut->Remove();

					if (Parent(ut) && Parent(next)) {
						ut = Parent(ut);
						next = Parent(next);
					}
					else {
						break;
					}
				}
			}

			static bool __LoadData6(VECTOR<wiz::Token2>* _strVec, int start_idx, int last_idx, Node* _global, const wiz::LoadDataOption* _option,
				int start_state, int last_state, Node** next) // first, strVec.empty() must be true!!
			{
				static wiz::Token2 EmptyToken;
				EmptyToken.str = nullptr;
				EmptyToken.len = 0;
				static wiz::Token2 SharpToken;
				static char sharp = '#';
				SharpToken.str = &sharp;
				SharpToken.len = 1;

				std::vector<wiz::Token2> varVec;
				std::vector<wiz::Token2> valVec;

				if (start_idx > last_idx) {
					return false;
				}

				VECTOR<wiz::Token2>& strVec = *_strVec;
				Node& global = *_global;
				const wiz::LoadDataOption& option = *_option;

				int state = start_state;
				int braceNum = 0;
				std::vector< Node* > nestedUT(1);
				wiz::Token2 var = EmptyToken, val = EmptyToken;


				nestedUT[0] = &global;

				int i = start_idx;

				while (false == strVec.empty() && i <= last_idx) {
					switch (state)
					{
					case 0:
					{
						if (strVec[i].len == 1 && -1 != Equal(option.Left, strVec[i].str[0])) {
							i += 1;

							for (int i = 0; i < varVec.size(); ++i) {
								AddItem(nestedUT[braceNum], varVec[i], valVec[i]);
							}
							varVec.clear();
							valVec.clear();

							AddUserType(nestedUT[braceNum], EmptyToken);
							Node* pTemp = nullptr;

							pTemp = nestedUT[braceNum]->first_child->last;
							//nestedUT[braceNum]->GetLastUserTypeItemRef("", pTemp);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
								nestedUT.push_back(nullptr);
							}

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp;
							///

							state = 0;
						}
						else if (strVec[i].len == 1 && -1 != Equal(option.Right, strVec[i].str[0])) {
							i += 1;

							state = 0;

							for (int i = 0; i < varVec.size(); ++i) {
								AddItem(nestedUT[braceNum], varVec[i], valVec[i]);
							}
							varVec.clear();
							valVec.clear();

							if (braceNum == 0) {
								Node ut;
								AddUserType(&ut, SharpToken); // json -> "var_name" = val  // clautext, # is line comment delimiter.
								Node* pTemp = nullptr;
								pTemp = ut.first_child->last;
								//ut.GetLastUserTypeItemRef("#", pTemp);
								
								Node* iter = nestedUT[braceNum]->first_child;
								Node* last = iter->last;
								Node* next_iter = iter->next;
								for (; iter != nullptr; iter = next_iter) {
									next_iter = iter->next;

									if (iter->is_ut) {
										AddUserType(ut.first_child, iter);
										//ut.GetUserTypeList(0)->AddUserTypeItem(std::move(*(nestedUT[braceNum]->GetUserTypeList(utCount))));
									}
									else {
										AddItem(ut.first_child, iter);
									}
								}

								nestedUT[braceNum]->first_child = nullptr;
								nestedUT[braceNum]->size = 0;
								nestedUT[braceNum]->ut_size = 0;
								//nestedUT[braceNum]->Remove();
								
								AddUserType(nestedUT[braceNum], ut.first_child);
								//nestedUT[braceNum]->AddUserTypeItem(std::move(*(ut.GetUserTypeList(0))));

								braceNum++;
							}

							{
								if (braceNum < nestedUT.size()) {
									nestedUT[braceNum] = nullptr;
								}
								braceNum--;
							}
						}
						else {
							std::pair<bool, Token2> bsPair;

							if (i < last_idx)
							{
								bsPair = std::make_pair(true, strVec[i + 1]);
							}
							else {
								bsPair = std::make_pair(false, Token2());
							}

							if (bsPair.first) {
								if (bsPair.second.len == 1 && -1 != Equal(option.Assignment, bsPair.second.str[0])) {
									// var2
									var = strVec[i];
									state = 1;
									i += 1;
									i += 1;
								}
								else {
									// var1
									if (i <= last_idx) {
										val = strVec[i];

										varVec.push_back(var);
										valVec.push_back(val);
										//nestedUT[braceNum]->AddItem("", ""); // std::move(val));
										val = EmptyToken; // "";
										var = EmptyToken;
										state = 0;
										i += 1;
									}
								}
							}
							else
							{
								// var1
								if (i <= last_idx)
								{
									val = strVec[i];
									varVec.push_back(var);
									valVec.push_back(val);
									//nestedUT[braceNum]->AddItem("", "");// std::move(val));
									var = EmptyToken;
									val = EmptyToken;

									state = 0;
									i += 1;
								}
							}
						}
					}
					break;
					case 1:
					{
						if (strVec[i].len == 1 && -1 != Equal(option.Left, strVec[i].str[0])) {
							i += 1;
							
							for (int i = 0; i < varVec.size(); ++i) {
								AddItem(nestedUT[braceNum], varVec[i], valVec[i]);
								//nestedUT[braceNum]->AddItem(std::move(varVec[i]), std::move(valVec[i]));
							}
							varVec.clear();
							valVec.clear();


							///
							{
								AddUserType(nestedUT[braceNum], var);
								Node* pTemp = nullptr;
								pTemp = nestedUT[braceNum]->first_child->last;
								//nestedUT[braceNum]->GetLastUserTypeItemRef(var, pTemp);
								var = EmptyToken; // "";
								braceNum++;

								/// new nestedUT
								if (nestedUT.size() == braceNum) {
									nestedUT.push_back(nullptr);
								}

								/// initial new nestedUT.
								nestedUT[braceNum] = pTemp;
							}
							///
							state = 0;
						}
						else {
							if (i <= last_idx) {
								val = strVec[i]; // std::string(strVec[i].str, strVec[i].len);

								i += 1;
								varVec.push_back(var);
								valVec.push_back(val);
								//nestedUT[braceNum]->AddItem("", ""); // std::move(var), std::move(val));
								var = EmptyToken; val = EmptyToken;

								state = 0;
							}
						}
					}
					break;
					default:
						// syntax err!!
						throw "syntax error ";
						break;
					}
				}


				if (varVec.empty() == false) {
					for (int i = 0; i < varVec.size(); ++i) {
						AddItem(nestedUT[braceNum], varVec[i], valVec[i]);
					}
					varVec.clear();
					valVec.clear();
				}
				
				if (next) {
					*next = nestedUT[braceNum];
				}

				if (state != last_state) {
					throw std::string("error final state is not last_state!  : ") + toStr(state);
				}
				if (i > last_idx + 1) {
					throw std::string("error i > last_idx + 1: " + toStr(i) + " , " + toStr(last_idx));
				}
				return true;
			}

		private:
			// [must] option.Assignment[i].size() == 1  - chk!
			static int FindRight2(VECTOR<Token2> & strVec, int start, int last, const wiz::LoadDataOption & option)
			{
				for (int i = last; i >= start; --i) {
					if (strVec[i].len == 1 && -1 != Equal(option.Right, strVec[i].str[0])) {
						return i;
					}

					if (strVec[i].len == 1 && -1 != Equal(option.Left, strVec[i].str[0])) {
						return i;
					}
					else if (strVec[i].len == 1 && -1 != Equal(option.Assignment, strVec[i].str[0])) {
						//
					}
					else if (i < last && !(strVec[i + 1].len == 1 && -1 != Equal(option.Assignment, strVec[i + 1].str[0])))
					{                // NOT
						return i;
					}
				}

				return -1;
			}

		public:
			//// ToDo : # => option.~?
			template <class Reserver>
			static bool _LoadData5(VECTOR<Token2> & strVec, Reserver & reserver, UserType & global, const wiz::LoadDataOption & option, const int lex_thr_num, const int parse_num) // first, strVec.empty() must be true!!
			{
				const int pivot_num = parse_num - 1;
				char* buffer = nullptr;

				bool end = false;
				{
					int a = clock();
					end = !reserver(&strVec, option, lex_thr_num, buffer);
					int b = clock();
					std::cout << b - a << "ms ";
				}

				UserType* before_next = nullptr;
				UserType _global;

				bool first = true;
				long long sum = 0;
int a, c;
				while (true) {
					end = true;

					
					a = clock();
					
					std::set<int> _pivots;
					std::vector<int> pivots;
					const int last_idx = strVec.size() - 1;

					if (pivot_num > 0) {
						std::vector<int> pivot;

						for (int i = 0; i < pivot_num; ++i) {
							pivot.push_back(FindRight2(strVec, (last_idx / (pivot_num + 1)) * (i), (last_idx / (pivot_num + 1)) * (i + 1) - 1, option));
						}

						for (int i = 0; i < pivot.size(); ++i) {
							if (pivot[i] != -1) {
								_pivots.insert(pivot[i]);
							}
						}

						for (auto& x : _pivots) {
							pivots.push_back(x);
						}
					}

					std::vector<UserType*> next(pivots.size() + 1, nullptr);

					{
						std::vector<UserType> __global(pivots.size() + 1);

						std::vector<std::thread> thr(pivots.size() + 1);

						{
							int idx = pivots.empty() ? last_idx : pivots[0];
							thr[0] = std::thread(__LoadData5, &strVec, 0, idx, &__global[0], &option, 0, 0, &next[0]);
							// __LoadData4 -> __LoadData5
						}

						for (int i = 1; i < pivots.size(); ++i) {
							thr[i] = std::thread(__LoadData5, &strVec, pivots[i - 1] + 1, pivots[i], &__global[i], &option, 0, 0, &next[i]);
						}

						if (pivots.size() >= 1) {
							thr[pivots.size()] = std::thread(__LoadData5, &strVec, pivots.back() + 1, last_idx, &__global[pivots.size()],
								&option, 0, 0, &next[pivots.size()]);
						}

						// wait
						for (int i = 0; i < thr.size(); ++i) {
							thr[i].join();
						}
						c = clock();
						// Merge
						try {
							if (first) {
								Merge(&_global, &__global[0], &next[0]);
								first = false;
							}
							else {
								Merge(before_next, &__global[0], &next[0]);
							}

							for (int i = 1; i < pivots.size() + 1; ++i) {
								Merge(next[i - 1], &__global[i], &next[i]);
							}

						}
						catch (...) {
							delete[] buffer;
							buffer = nullptr;
							throw "in Merge, error";
						}
						{
							if (next[pivots.size()] != nullptr && next[pivots.size()]->GetParent() != nullptr) {
								delete[] buffer;
								buffer = nullptr;
								throw "merge error";
							}
						}

						strVec.erase(strVec.begin(), strVec.begin() + (last_idx + 1));

						before_next = next.back();
					}

					
					if (!end) {
						//
					}
					else {
						break;
					}
				}
int b = clock();
				delete[] buffer;

				global = std::move(_global);
				std::cout << b - a << "ms" << " " << b - c << "ms" << "\n";
					

				return true;
			}
			template <class Reserver>
			static bool _LoadData6(VECTOR<Token2>& strVec, Reserver& reserver, Node& global, const wiz::LoadDataOption& option, const int lex_thr_num, const int parse_num, char** out_buffer) // first, strVec.empty() must be true!!
			{
				const int pivot_num = parse_num - 1;
				char* buffer = nullptr;

				bool end = false;
				{
					int a = clock();
					end = !reserver(&strVec, option, lex_thr_num, buffer);
					int b = clock();
					std::cout << b - a << "ms ";
				}

				Node* before_next = nullptr;
				Node _global;

				bool first = true;
				long long sum = 0;

				while (true) {
					end = true;

					int a = clock();
					int c;

					std::set<int> _pivots;
					std::vector<int> pivots;
					const int last_idx = strVec.size() - 1;

					if (pivot_num > 0) {
						std::vector<int> pivot;

						for (int i = 0; i < pivot_num; ++i) {
							pivot.push_back(FindRight2(strVec, (last_idx / (pivot_num + 1)) * (i), (last_idx / (pivot_num + 1)) * (i + 1) - 1, option));
						}

						for (int i = 0; i < pivot.size(); ++i) {
							if (pivot[i] != -1) {
								_pivots.insert(pivot[i]);
							}
						}

						for (auto& x : _pivots) {
							pivots.push_back(x);
						}
					}

					std::vector<Node*> next(pivots.size() + 1, nullptr);

					{
						std::vector<Node> __global(pivots.size() + 1);

						std::vector<std::thread> thr(pivots.size() + 1);

						{
							int idx = pivots.empty() ? last_idx : pivots[0];
							thr[0] = std::thread(__LoadData6, &strVec, 0, idx, &__global[0], &option, 0, 0, &next[0]);
						}

						for (int i = 1; i < pivots.size(); ++i) {
							thr[i] = std::thread(__LoadData6, &strVec, pivots[i - 1] + 1, pivots[i], &__global[i], &option, 0, 0, &next[i]);
						}

						if (pivots.size() >= 1) {
							thr[pivots.size()] = std::thread(__LoadData6, &strVec, pivots.back() + 1, last_idx, &__global[pivots.size()],
								&option, 0, 0, &next[pivots.size()]);
						}

						// wait
						for (int i = 0; i < thr.size(); ++i) {
							thr[i].join();
						}
					
						c = clock();
						// Merge
						try 
						{
							if (first) {
								Merge2(&_global, &__global[0], &next[0]);
								first = false;
							}
							else {
								Merge2(before_next, &__global[0], &next[0]);
							}

							for (int i = 1; i < pivots.size() + 1; ++i) {
								Merge2(next[i - 1], &__global[i], &next[i]);
							}

						}
						catch (...) {
							delete[] buffer;
							buffer = nullptr;
							throw "in Merge, error";
						}
						{
							if (next[pivots.size()] != nullptr && Parent(next[pivots.size()]) != nullptr) {
								delete[] buffer;
								buffer = nullptr;
								throw "merge error";
							}
						}

						strVec.erase(strVec.begin(), strVec.begin() + (last_idx + 1));

						before_next = next.back();
					}

					int b = clock();
					std::cout << b - a << "ms";
					std::cout << " " << b - c << "ms\n";
					if (!end) {
						//
					}
					else {
						break;
					}
				}

				//delete[] buffer;
				*out_buffer = buffer;

				global = _global;

				return true;
			}

			static bool LoadDataFromFile(const std::string & fileName, UserType & global, int pivot_num = -1, int lex_thr_num = 0) /// global should be empty
			{
				if (pivot_num < 0) {
					pivot_num = std::thread::hardware_concurrency() - 1;
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = std::thread::hardware_concurrency();
				}
				if (pivot_num <= -1) {
					pivot_num = 0;
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = 1;
				}


				bool success = true;
				std::ifstream inFile;
				inFile.open(fileName, std::ios::binary);


				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}
				UserType globalTemp;
				static VECTOR<Token2> strVec;

				strVec.clear();

				try {
					InFileReserver3 ifReserver(inFile);
					wiz::LoadDataOption option;
					option.Assignment.push_back('=');
					option.Left.push_back('{');
					option.Right.push_back('}');
					option.LineComment.push_back("#");

					ifReserver.Num = 1 << 19;
					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData5(strVec, ifReserver, globalTemp, option, lex_thr_num, pivot_num + 1))
					{
						inFile.close();
						return false; // return true?
					}

					inFile.close();
				}
				catch (const char* err) { std::cout << err << std::endl; inFile.close(); return false; }
				catch (const std::string & e) { std::cout << e << std::endl; inFile.close(); return false; }
				catch (std::exception e) { std::cout << e.what() << std::endl; inFile.close(); return false; }
				catch (...) { std::cout << "not expected error" << std::endl; inFile.close(); return false; }

				global = std::move(globalTemp);
				return true;
			}	
			
			static bool LoadDataFromFile2(const std::string& fileName, Node& global, int pivot_num, int lex_thr_num, char** _buffer) /// global should be empty
			{
				if (pivot_num < 0) {
					pivot_num = std::thread::hardware_concurrency() - 1;
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = std::thread::hardware_concurrency();
				}
				if (pivot_num <= -1) {
					pivot_num = 0;
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = 1;
				}


				bool success = true;
				std::ifstream inFile;
				inFile.open(fileName, std::ios::binary);


				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}
				Node globalTemp;
				static VECTOR<Token2> strVec;

				strVec.clear();

				try {
					InFileReserver3 ifReserver(inFile);
					wiz::LoadDataOption option;
					option.Assignment.push_back('=');
					option.Left.push_back('{');
					option.Right.push_back('}');
					option.LineComment.push_back("#");

					char* buffer = nullptr;
					ifReserver.Num = 1 << 19;
					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData6(strVec, ifReserver, globalTemp, option, lex_thr_num, pivot_num + 1, _buffer))
					{
						inFile.close();
						return false; // return true?
					}

					inFile.close();
				}
				catch (const char* err) { std::cout << err << std::endl; inFile.close(); return false; }
				catch (const std::string & e) { std::cout << e << std::endl; inFile.close(); return false; }
				catch (std::exception e) { std::cout << e.what() << std::endl; inFile.close(); return false; }
				catch (...) { std::cout << "not expected error" << std::endl; inFile.close(); return false; }

				
				global = globalTemp;
				
				return true;
			}

		public:
			static bool LoadDataFromString(const std::string & str, UserType & ut)
			{

				UserType utTemp = ut;
				ARRAY_QUEUE<Token> strVec;

				wiz::StringBuilder builder(str.size(), str.c_str(), str.size());

				wiz::LoadDataOption option;
				option.Assignment.push_back('=');
				option.Left.push_back('{');

				//option.MuitipleLineCommentStart.push_back("###");
				//option.MuitipleLineCommentEnd.push_back("###");

				option.LineComment.push_back("#");
				option.Right.push_back('}');
				Utility::DoThread doThread(&builder, &strVec, &option);

				doThread();

				try {
					// empty std::string!
					NoneReserver nonReserver;
					if (false == _LoadData(strVec, nonReserver, utTemp, option))
					{
						return true;
					}
				}
				catch (const char* err) { std::cout << err << std::endl; return false; }
				catch (std::exception & e) { std::cout << e.what() << std::endl; return false; }
				catch (std::string str) { std::cout << str << std::endl; return false; }
				catch (...) { std::cout << "not expected error" << std::endl; return  false; }

				ut = std::move(utTemp);
				return true;
			}
		public:
			static bool LoadWizDB(UserType & global, const std::string & fileName, const int thr_num) {
				UserType globalTemp = UserType("global");

				// Scan + Parse 
				if (false == LoadDataFromFile(fileName, globalTemp, thr_num, thr_num)) { return false; }
				std::cout << "LoadData End" << std::endl;

				global = std::move(globalTemp);
				return true;
			}
			// SaveQuery
			static bool SaveWizDB(const UserType & global, const std::string & fileName, const bool append = false) {
				std::ofstream outFile;
				if (fileName.empty()) { return false; }
				if (false == append) {
					outFile.open(fileName);
					if (outFile.fail()) { return false; }
				}
				else {
					outFile.open(fileName, std::ios::app);
					if (outFile.fail()) { return false; }

					outFile << "\n";
				}

				/// saveFile
				global.Save1(outFile); // cf) friend

				outFile.close();

				return true;
			}
		};
