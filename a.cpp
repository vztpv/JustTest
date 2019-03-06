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
