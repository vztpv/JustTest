
	class UserType : public Type {
	private:
		class UserTypeCompare
		{
		public:
			bool operator() (const WrapType<UserType*> x, const WrapType<UserType*> y) const {
				return x.ptr->GetName() < y.ptr->GetName();
			}
		};
		class ItemTypeStringPtrCompare {
		public:
			bool operator() (const WrapType<ItemType<std::string>*> x, const WrapType<ItemType<std::string>*> y) const {
				return x.ptr->GetName() < y.ptr->GetName();
			}
		};

		int Dif(const std::string& x, const std::string& y) const {
			return x.compare(y); // strcmp
		}
		size_t binary_find_ut(const std::vector<WrapType<UserType*>>& arr, const UserType& x, bool& err) const
		{
			err = false;

			if (arr.empty()) { err = true;  return -1; }

			size_t left = 0, right = arr.size() - 1;
			size_t middle = (left + right) / 2;

			while (left <= right) {
				const int dif = Dif(arr[middle].ptr->GetName(), x.GetName());

				if (dif == 0) { //arr[middle]->GetName() == x.GetName()) {
					return middle;
				}
				else if (dif < 0) { //arr[middle]->GetName() < x.GetName()) {
					left = middle + 1;
				}
				else {
					if (0 == middle) {
						break;
					}
					right = middle - 1;
				}

				middle = (left + right) / 2;
			}
			err = true;
			return -1;
		}

		size_t binary_find_it(const std::vector<WrapType<ItemType<std::string>*>>&arr, const ItemType<std::string>& x, bool& err) const {
			err = false;
			if (arr.empty()) { err = true;  return -1; }

			size_t left = 0, right = arr.size() - 1;
			size_t middle = (left + right) / 2;

			while (left <= right) {
				const int dif = Dif(arr[middle].ptr->GetName(), x.GetName());

				if (dif == 0) { //arr[middle]->GetName() == x.GetName()) {
					return middle;
				}
				else if (dif < 0) { //arr[middle]->GetName() < x.GetName()) {
					left = middle + 1;
				}
				else {
					if (0 == middle) {
						break;
					}
					right = middle - 1;
				}

				middle = (left + right) / 2;
			}

			err = true;
			return -1;
		}

	public:
		size_t GetIListSize()const { return ilist.size(); }
		size_t GetItemListSize()const { return itemList.size(); }
		size_t GetUserTypeListSize()const { return userTypeList.size(); }
		ItemType<std::string>& GetItemList(const size_t idx) { return itemList[idx]; }
		const ItemType<std::string>& GetItemList(const size_t idx) const { return itemList[idx]; }
		UserType*& GetUserTypeList(const size_t idx) { return userTypeList[idx]; }
		const UserType*& GetUserTypeList(const size_t idx) const { return const_cast<const UserType*&>(userTypeList[idx]); }

		Type* ToType() {
			return this;
		}
		const Type* ToType()const {
			return this;
		}

		bool IsItemList(const size_t idx) const
		{
			return ilist[idx] == 1;
		}
		bool IsUserTypeList(const size_t idx) const
		{
			return ilist[idx] == 2;
		}
		/*
		void AddItemList(const ItemType<std::string>& strTa)
		{
			for (int i = 0; i < strTa.size(); ++i) {
				this->AddItem(strTa.GetName(), strTa.Get(i));
			}
		}
		void AddItemList(ItemType<std::string>&& strTa)
		{
			for (int i = 0; i < strTa.size(); ++i) {
				this->AddItem(std::move(strTa.GetName()), std::move(strTa.Get(i)));
			}
		}
		*/
	public:
		void Clear() {
			itemList.clear();
			ilist.clear();
			userTypeList.clear();

			sortedItemList.clear();
			sortedUserTypeList.clear();
		}

		void SetParent(UserType* other)
		{
			parent = other;
		}
		UserType* GetParent() { return parent; }
		const UserType* GetParent()const { return parent; }

		void LinkUserType(UserType* ut) // friend?
		{
			userTypeList.push_back(ut);
			ilist.push_back(2);
			ut->parent = this;

			useSortedUserTypeList = false;
		}
	private:
		UserType* parent = nullptr;
		std::vector<int> ilist;
		std::vector< ItemType<std::string> > itemList;
		std::vector< UserType* > userTypeList;
		mutable std::vector< WrapType<ItemType<std::string>*> > sortedItemList;
		mutable std::vector< WrapType<UserType*> > sortedUserTypeList;
		mutable bool useSortedItemList = false;
		mutable bool useSortedUserTypeList = false;
	public:
		explicit UserType(const char* str, size_t len) : Type(std::string(str, len)), parent(nullptr) { }
		explicit UserType(std::string&& name) : Type(std::move(name)), parent(nullptr) { }
		explicit UserType(const std::string& name = "") : Type(name), parent(nullptr) { }
		UserType(const UserType& ut) : Type(ut.GetName()) {
			Reset(ut);  // Initial
		}
		UserType(UserType&& ut) noexcept : Type(std::move(ut.GetName())) {
			Reset2(std::move(ut));
		}
		virtual ~UserType() {
			_Remove();
		}
		UserType& operator=(const UserType& ut) {
			if (this == &ut) { return *this; }
			Type::operator=(ut);

			RemoveUserTypeList();
			Reset(ut);
			return *this;
		}
		void operator=(UserType&& ut) noexcept {
			if (this == &ut) { return; }

			Type::operator=(ut);
			RemoveUserTypeList();
			Reset2(std::move(ut));
			return;
		}
		class Wrap {
		public:
			UserType* ut = nullptr;
			size_t idx = 0;
			size_t max = 0;
		public:
			Wrap() {
				//
			}
			Wrap(UserType* ut) :
				ut(ut)
			{
				max = ut->GetUserTypeListSize();
			}
		};
		class Wrap2 {
		public:
			const UserType* ut = nullptr;
			size_t idx = 0;
			size_t max = 0;
		public:
			Wrap2() {
				//
			}
			Wrap2(const UserType* ut) :
				ut(ut)
			{
				max = ut->GetUserTypeListSize();
			}
		};

		static void Delete(void* ptr) {
			std::vector<Wrap> _stack;
			_stack.push_back(Wrap((UserType*)ptr));

			while (!_stack.empty()) {
				if (_stack.back().idx >= _stack.back().max) {
					_stack.back().ut->userTypeList.clear();
					delete _stack.back().ut;

					_stack.pop_back();
					if (_stack.empty()) {
						break;
					}
					_stack.back().idx++;
					continue;
				}

				_stack.push_back(Wrap(_stack.back().ut->GetUserTypeList(_stack.back().idx)));
			}
		}

	private:
		void Reset(const UserType& ut) {
			std::vector<UserType*> _stack;
			std::vector<Wrap2> _stack2;

			_stack.push_back(this);
			_stack2.push_back(Wrap2((const UserType*)&ut));

			while (!_stack2.empty()) {
				if (_stack2.back().idx >= _stack2.back().max) {
					{
						_stack.back()->sortedItemList.clear();
						_stack.back()->sortedUserTypeList.clear();
						_stack.back()->name = _stack2.back().ut->name;
						_stack.back()->ilist = _stack2.back().ut->ilist;
						_stack.back()->itemList = _stack2.back().ut->itemList;
						_stack.back()->useSortedItemList = false; // ut.useSortedItemList;
						_stack.back()->useSortedUserTypeList = false; //ut.useSortedUserTypeList;
					}

					{
						UserType* child = _stack.back();

						_stack.pop_back();

						if (!_stack.empty()) {
							_stack.back()->LinkUserType(child);
						}
					}

					_stack2.pop_back();
					if (_stack2.empty()) {
						break;
					}
					_stack2.back().idx++;
					continue;
				}

				{
					UserType* child = new UserType(_stack2.back().ut->GetName());
					_stack.push_back(child);
				}
				_stack2.push_back(Wrap2(_stack2.back().ut->GetUserTypeList(_stack2.back().idx)));
				_stack.back()->ReserveUserTypeList(_stack2.back().ut->GetUserTypeListSize());
			}
		}
		void Reset2(UserType&& ut) {
			ilist = std::move(ut.ilist);
			itemList = std::move(ut.itemList);

			//sortedItemList = std::move(ut.sortedItemList);
			//sortedUserTypeList = std::move(ut.sortedUserTypeList);

			useSortedItemList = false; // ut.useSortedItemList;
			useSortedUserTypeList = false; // ut.useSortedUserTypeList;

			userTypeList.reserve(ut.userTypeList.size());

			for (size_t i = 0; i < ut.userTypeList.size(); ++i) {
				userTypeList.push_back(std::move(ut.userTypeList[i]));
				ut.userTypeList[i] = nullptr;
				userTypeList.back()->parent = this;
			}
			ut.userTypeList.clear();


			sortedItemList.clear();
			sortedUserTypeList.clear();

			ut.sortedItemList.clear();
			ut.sortedUserTypeList.clear();
		}

		void _Remove()
		{
			//parent = nullptr;
			ilist = std::vector<int>();
			itemList = std::vector< ItemType<std::string> >();

			sortedItemList.clear();
			sortedUserTypeList.clear();

			useSortedItemList = false;
			useSortedUserTypeList = false;
			RemoveUserTypeList();
		}
	public:
		size_t  GetIlistIndex(const size_t  index, const int type, bool& err)
		{
			return _GetIlistIndex(ilist, index, type, err);
		}
		size_t GetUserTypeIndexFromIlistIndex(const size_t ilist_idx, bool& err)
		{
			return _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx, err);
		}
		size_t GetItemIndexFromIlistIndex(const size_t  ilist_idx, bool& err)
		{
			return _GetItemIndexFromIlistIndex(ilist, ilist_idx, err);
		}
	private:
		/// val : 1 or 2
		size_t  _GetIndex(const std::vector<int>& ilist, const int val, bool& err, const size_t  start = 0) {
			err = false;
			for (size_t i = start; i < ilist.size(); ++i) {
				if (ilist[i] == val) { return i; }
			}
			err = true;
			return -1;
		}
		// test? - need more thinking!
		size_t  _GetItemIndexFromIlistIndex(const std::vector<int>& ilist, const size_t  ilist_idx, bool& _err) {
			if (ilist.size() == ilist_idx) { return ilist.size(); }
			bool err = false;
			_err = false;
			size_t idx = _GetIndex(ilist, 1, err, 0);
			size_t item_idx = -1;

			while (!err) {
				item_idx++;
				if (ilist_idx == idx) { return item_idx; }
				idx = _GetIndex(ilist, 1, err, idx + 1);
			}
			_err = true;
			return -1;
		}
		size_t  _GetUserTypeIndexFromIlistIndex(const std::vector<int>& ilist, const size_t  ilist_idx, bool& _err) {
			_err = false;
			bool err = false;
			if (ilist.size() == ilist_idx) { return ilist.size(); }
			size_t  idx = _GetIndex(ilist, 2, err, 0);
			size_t  usertype_idx = -1;

			while (!err) {
				usertype_idx++;
				if (ilist_idx == idx) { return usertype_idx; }
				idx = _GetIndex(ilist, 2, err, idx + 1);
			}
			_err = true;
			return -1;
		}
		/// type : 1 or 2
		size_t _GetIlistIndex(const std::vector<int>& ilist, const size_t  index, const int type, bool& err) {
			size_t  count = -1;
			err = false;
			for (size_t i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == type) {
					count++;
					if (index == count) {
						return i;
					}
				}
			}
			err = true;
			return -1;
		}
	public:
		void RemoveItemList(const size_t  idx)
		{
			// left shift start idx, to end, at itemList. and resize!
			for (size_t i = idx + 1; i < GetItemListSize(); ++i) {
				itemList[i - 1] = std::move(itemList[i]);
			}
			itemList.resize(itemList.size() - 1);
			//  ilist left shift and resize - count itemType!
			size_t count = 0;
			for (size_t i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 1) { count++; }
				if (count == idx + 1) {
					// left shift!and resize!
					for (size_t k = i + 1; k < ilist.size(); ++k) {
						ilist[k - 1] = std::move(ilist[k]);
					}
					ilist.resize(ilist.size() - 1);
					break;
				}
			}

			useSortedItemList = false;
		}
		void RemoveUserTypeList(const size_t idx, const bool chk = true)
		{
			if (chk && userTypeList[idx]) {
				delete userTypeList[idx];
			}

			// left shift start idx, to end, at itemList. and resize!
			for (size_t i = idx + 1; i < GetUserTypeListSize(); ++i) {
				userTypeList[i - 1] = std::move(userTypeList[i]);
			}
			userTypeList.resize(userTypeList.size() - 1);
			//  ilist left shift and resize - count itemType!
			size_t  count = 0;
			for (size_t i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 2) { count++; }
				if (count == idx + 1) {
					// left shift! and resize!
					for (size_t k = i + 1; k < ilist.size(); ++k) {
						ilist[k - 1] = std::move(ilist[k]);
					}
					ilist.resize(ilist.size() - 1);
					break;
				}
			}

			useSortedUserTypeList = false;
		}
		void RemoveItemList(const std::string& varName)
		{
			bool err = false;
			size_t k = _GetIndex(ilist, 1, err, 0);

			std::vector<ItemType<std::string>> tempDic;
			for (size_t i = 0; i < itemList.size(); ++i) {
				if (varName != itemList[i].GetName()) {
					tempDic.push_back(itemList[i]);
					k = _GetIndex(ilist, 1, err, k + 1);
				}
				else {
					// remove item, ilist left shift 1.
					for (size_t j = k + 1; j < ilist.size(); ++j) {
						ilist[j - 1] = ilist[j];
					}
					ilist.resize(ilist.size() - 1);
					k = _GetIndex(ilist, 1, err, k);
				}
			}
			itemList = std::move(tempDic);

			useSortedItemList = false;
		}
		void RemoveItemList() /// ALL
		{
			itemList = std::vector<ItemType<std::string>>();
			//
			std::vector<int> temp;
			for (size_t i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 2)
				{
					temp.push_back(2);
				}
			}
			ilist = move(temp);

			useSortedItemList = false;
		}
		void RemoveEmptyItem()
		{
			bool err = false;
			size_t k = _GetIndex(ilist, 1, err, 0);
			std::vector<ItemType<std::string>> tempDic;
			for (size_t i = 0; i < itemList.size(); ++i) {
				if (itemList[i].size() > 0) {
					tempDic.push_back(itemList[i]);
					k = _GetIndex(ilist, 1, err, k + 1);
				}
				else {
					// remove item, ilist left shift 1.
					for (size_t j = k + 1; j < ilist.size(); ++j) {
						ilist[j - 1] = ilist[j];
					}
					ilist.resize(ilist.size() - 1);
					k = _GetIndex(ilist, 1, err, k);
				}
			}
			itemList = move(tempDic);

			useSortedItemList = false;
		}
		void Remove()
		{
			ilist = std::vector<int>();
			itemList = std::vector< ItemType<std::string> >();

			RemoveUserTypeList();

			sortedItemList.clear();
			sortedUserTypeList.clear();

			useSortedItemList = false;
			useSortedUserTypeList = false;
		}
		void RemoveUserTypeList() {
			for (size_t i = 0; i < userTypeList.size(); i++) {
				if (nullptr != userTypeList[i]) {
					Delete(userTypeList[i]); //
					userTypeList[i] = nullptr;
				}
			}

			userTypeList.clear();

			std::vector<int> temp;
			for (size_t i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 1)
				{
					temp.push_back(1);
				}
			}
			ilist = move(temp);

			useSortedUserTypeList = false;
		}
		void RemoveUserTypeList(const std::string& varName, const bool chk = true)
		{
			bool err = false;
			size_t k = _GetIndex(ilist, 2, err, 0);
			std::vector<UserType*> tempDic;
			for (size_t i = 0; i < userTypeList.size(); ++i) {
				if (userTypeList[i] && varName != userTypeList[i]->GetName()) {
					tempDic.push_back(userTypeList[i]);
					k = _GetIndex(ilist, 2, err, k + 1);
				}
				else {
					if (chk && userTypeList[i]) {
						delete userTypeList[i];
					}
					// remove usertypeitem, ilist left shift 1.
					for (size_t j = k + 1; j < ilist.size(); ++j) {
						ilist[j - 1] = ilist[j];
					}
					ilist.resize(ilist.size() - 1);
					k = _GetIndex(ilist, 2, err, k);
				}
			}
			userTypeList = move(tempDic);

			useSortedUserTypeList = false;
		}
		//			
		void RemoveList(const size_t idx) // ilist_idx!
		{
			// chk whether item or usertype.
			// find item_idx or usertype_idx.
			// remove item or remove usertype.
			if (ilist[idx] == 1) {
				size_t  item_idx = 0; // -1

				for (size_t i = 0; i < ilist.size() && i <= idx; ++i) {
					if (ilist[i] == 1) { item_idx++; }
				}

				RemoveItemList(item_idx - 1); // item_idx
			}
			else {
				size_t  usertype_idx = 0;

				for (size_t i = 0; i < ilist.size() && i <= idx; ++i) {
					if (ilist[i] == 2) { usertype_idx++; }
				}

				RemoveUserTypeList(usertype_idx - 1);
			}
		}
	public:
		bool empty()const { return ilist.empty(); }

		void InsertItemByIlist(const size_t ilist_idx, const std::string& name, const std::string& item) {
			ilist.push_back(1);
			for (size_t i = ilist.size(); i > ilist_idx + 1; --i) {
				ilist[i - 1] = ilist[i - 2];
			}
			ilist[ilist_idx] = 1;

			bool err = false;
			size_t  itemIndex = _GetItemIndexFromIlistIndex(ilist, ilist_idx, err);

			itemList.emplace_back("", std::string(""));

			if (!err) {
				for (size_t i = itemList.size(); i > itemIndex + 1; --i) {
					itemList[i - 1] = std::move(itemList[i - 2]);
				}
				itemList[itemIndex] = ItemType<std::string>(name, item);
			}
			else {
				itemList[0] = ItemType<std::string>(name, item);
			}

			useSortedItemList = false;
		}
		void InsertItemByIlist(const size_t  ilist_idx, std::string&& name, std::string&& item) {
			ilist.push_back(1);

			bool err = false;

			for (size_t i = ilist.size(); i > ilist_idx + 1; --i) {
				ilist[i - 1] = ilist[i - 2];
			}
			ilist[ilist_idx] = 1;

			size_t  itemIndex = _GetItemIndexFromIlistIndex(ilist, ilist_idx, err);

			itemList.emplace_back("", std::string(""));
			if (!err) {
				for (size_t i = itemList.size(); i > itemIndex + 1; --i) {
					itemList[i - 1] = std::move(itemList[i - 2]);
				}
				itemList[itemIndex] = ItemType<std::string>(std::move(name), std::move(item));
			}
			else {
				itemList[0] = ItemType<std::string>(std::move(name), std::move(item));
			}


			useSortedItemList = false;
		}

		void InsertUserTypeByIlist(const size_t ilist_idx, UserType&& item) {
			ilist.push_back(2);
			UserType* temp = new UserType(std::move(item));

			temp->parent = this;

			for (size_t i = ilist.size(); i > ilist_idx + 1; --i) {
				ilist[i - 1] = ilist[i - 2];
			}
			ilist[ilist_idx] = 2;

			bool err = false;
			size_t userTypeIndex = _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx, err);
			userTypeList.push_back(nullptr);
			if (!err) {
				for (size_t i = userTypeList.size(); i > userTypeIndex + 1; --i) {
					userTypeList[i - 1] = std::move(userTypeList[i - 2]);
				}
				userTypeList[userTypeIndex] = temp;
			}
			else {
				userTypeList[0] = temp;
			}


			useSortedUserTypeList = false;
		}
		void InsertUserTypeByIlist(const size_t ilist_idx, const UserType& item) {
			ilist.push_back(2);
			UserType* temp = new UserType(item);

			temp->parent = this;


			for (size_t i = ilist.size(); i > ilist_idx + 1; --i) {
				ilist[i - 1] = ilist[i - 2];
			}
			ilist[ilist_idx] = 2;

			bool err = false;
			size_t  userTypeIndex = _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx, err);
			userTypeList.push_back(nullptr);
			if (!err) {
				for (size_t i = userTypeList.size(); i > userTypeIndex + 1; --i) {
					userTypeList[i - 1] = std::move(userTypeList[i - 2]);
				}
				userTypeList[userTypeIndex] = temp;
			}
			else {
				userTypeList[0] = temp;
			}

			useSortedUserTypeList = false;
		}

		void ReserveIList(size_t offset)
		{
			if (offset > 0) {
				ilist.reserve(offset);
			}
		}
		void ReserveItemList(size_t offset)
		{
			if (offset > 0) {
				itemList.reserve(offset);
			}
		}
		void ReserveUserTypeList(size_t offset)
		{
			if (offset > 0) {
				userTypeList.reserve(offset);
			}
		}
		void AddItem(const char* str1, size_t len1, const char* str2, size_t len2) {
			itemList.emplace_back(str1, len1, str2, len2);
			ilist.push_back(1);

			useSortedItemList = false;
		}

		void AddItem(std::string&& name, std::string&& item) {
			itemList.emplace_back(std::move(name), std::move(item));
			ilist.push_back(1);

			useSortedItemList = false;
		}
		void AddItem(const std::string& name, const std::string& item) {
			itemList.emplace_back(name, item);
			ilist.push_back(1);

			useSortedItemList = false;
		}
		void AddItemType(clau_parser11::ItemType<std::string>&& item) {
			itemList.push_back(std::move(item));
			ilist.push_back(1);

			useSortedItemList = false;
		}

		void AddItemType(const clau_parser11::ItemType<std::string>& item) {
			itemList.push_back(item);
			ilist.push_back(1);

			useSortedItemList = false;
		}
		void AddUserTypeItem(UserType&& item) {
			UserType* temp = new UserType(std::move(item.GetName()));

			temp->parent = this;

			ilist.push_back(2);

			userTypeList.push_back(temp);

			useSortedUserTypeList = false;
			useSortedItemList = false;


			{
				temp->itemList = std::move(item.itemList);
				temp->ilist = std::move(item.ilist);
				temp->userTypeList = std::move(item.userTypeList);

				for (size_t i = 0; i < item.GetUserTypeListSize(); ++i) {
					item.userTypeList[i] = nullptr;
				}
				for (size_t i = 0; i < temp->GetUserTypeListSize(); ++i) {
					temp->userTypeList[i]->parent = temp;
				}
			}
		}
		void AddUserTypeItem(const UserType& item) {
			UserType* temp = new UserType(item);
			temp->parent = this;

			ilist.push_back(2);

			userTypeList.push_back(temp);

			useSortedUserTypeList = false;
		}
		void AddItemAtFront(std::string&& name, std::string&& item) {
			itemList.emplace(itemList.begin(), name, item);

			ilist.insert(ilist.begin(), 1);

			useSortedItemList = false;
		}
		void AddItemAtFront(const std::string& name, const std::string& item) {
			itemList.emplace(itemList.begin(), name, item);

			ilist.insert(ilist.begin(), 1);

			useSortedItemList = false;
		}
		void AddUserTypeItemAtFront(const UserType& item) {
			UserType* temp = new UserType(item);
			temp->parent = this;

			ilist.insert(ilist.begin(), 2);

			userTypeList.insert(userTypeList.begin(), temp);

			useSortedUserTypeList = false;
		}
		void AddUserTypeItemAtFront(UserType&& item) {
			UserType* temp = new UserType(item);
			temp->parent = this;

			ilist.insert(ilist.begin(), 2);

			userTypeList.insert(userTypeList.begin(), temp);

			useSortedUserTypeList = false;
		}


		std::vector<int> GetUserTypeIdx(const std::string& name) const {
			std::vector<int> temp;

			if (name == "*") {
				std::cout << userTypeList.size() << "|\n";
				for (size_t i = 0; i < userTypeList.size(); ++i) {
					temp.push_back(i);
				}
				return temp;
			}

			if (false == useSortedUserTypeList) {
				// make sortedUserTypeList.
				sortedUserTypeList.clear();
				for (size_t i = 0; i < userTypeList.size(); ++i) {
					sortedUserTypeList.emplace_back(userTypeList[i], i);

				}
				//	sortedUserTypeList = userTypeList;

				std::stable_sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

				useSortedUserTypeList = true;
			}
			// binary search
			{
				UserType x = UserType(name);
				bool err = false;
				size_t idx = binary_find_ut(sortedUserTypeList, x, err);
				if (!err) {
					size_t start = idx;
					size_t  last = idx;

					for (size_t i = idx; i > 0; --i) {
						if (name == sortedUserTypeList[i - 1].ptr->GetName()) {
							start--;
						}
						else {
							break;
						}
					}
					for (size_t i = idx + 1; i < sortedUserTypeList.size(); ++i) {
						if (name == sortedUserTypeList[i].ptr->GetName()) {
							last++;
						}
						else {
							break;
						}
					}

					for (size_t i = start; i < last + 1; ++i) {
						temp.push_back(sortedUserTypeList[i].idx);
					}
				}
				else {
					//std::cout << "no found" << "\n";
				}
			}

			return temp;
		}

		std::vector<ItemType<std::string>> GetItem(const std::string& name) const {
			std::vector<ItemType<std::string>> temp;
			/*if (String::startsWith(name, "$.") && name.size() >= 5) {
				// later, change to binary search?
				std::string str = name.substr(3, name.size() - 4);
				std::regex rgx(str);
				for (int i = 0; i < itemList.size(); ++i) {
					if (regex_match(itemList[i].GetName(), rgx)) {
						temp.push_back(itemList[i]);
					}
				}
			}
			else*/
			{
				if (false == useSortedItemList) {
					sortedItemList.clear();
					for (size_t i = 0; i < itemList.size(); ++i) {
						sortedItemList.push_back(WrapType<ItemType<std::string>*>((ItemType<std::string>*)&itemList[i], i));
					}

					std::sort(sortedItemList.begin(), sortedItemList.end(), ItemTypeStringPtrCompare());

					useSortedItemList = true;
				}
				// binary search
				{
					bool err = false;
					ItemType<std::string> x = ItemType<std::string>(name, "");
					size_t  idx = binary_find_it(sortedItemList, x, err);
					if (!err) {
						size_t  start = idx;
						size_t  last = idx;

						for (size_t i = idx; i > 0; --i) {
							if (name == sortedItemList[i - 1].ptr->GetName()) {
								start--;
							}
							else {
								break;
							}
						}
						for (size_t i = idx + 1; i < sortedItemList.size(); ++i) {
							if (name == sortedItemList[i].ptr->GetName()) {
								last++;
							}
							else {
								break;
							}
						}

						for (size_t i = start; i < last + 1; ++i) {
							temp.push_back(*sortedItemList[i].ptr);
						}
					}
					else {
						//std::cout << "no found" << "\n";
					}
				}
			}
			return temp;
		}
		// regex to SetItem?
		bool SetItem(const std::string& name, const std::string& value) {
			size_t index = -1;
			bool err = true;

			for (size_t i = 0; i < itemList.size(); ++i) {
				if (itemList[i].GetName() == name)
				{
					itemList[i].Set(0, value);
					index = i;
					err = false;
				}
			}

			return err;
		}
		/// add set Data
		bool SetItem(const size_t  var_idx, const std::string& value) {
			itemList[var_idx].Set(0, value);
			return true;
		}

		//// O(N) -> O(logN)?
		std::vector<UserType*> GetUserTypeItem(const std::string& name) const { /// chk...
			std::vector<UserType*> temp;

			if (false == useSortedUserTypeList) {
				// make sortedUserTypeList.
				for (size_t i = 0; i < userTypeList.size(); ++i) {
					sortedUserTypeList.emplace_back(userTypeList[i], i);
				}

				std::sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

				useSortedUserTypeList = true;
			}
			// binary search
			{
				UserType x = UserType(name);
				bool err = false;
				size_t idx = binary_find_ut(sortedUserTypeList, x, err);
				if (!err) {
					size_t start = idx;
					size_t  last = idx;

					for (size_t i = idx; i > 0; --i) {
						if (name == sortedUserTypeList[i - 1].ptr->GetName()) {
							start--;
						}
						else {
							break;
						}
					}
					for (size_t i = idx + 1; i < sortedUserTypeList.size(); ++i) {
						if (name == sortedUserTypeList[i].ptr->GetName()) {
							last++;
						}
						else {
							break;
						}
					}

					for (size_t i = start; i < last + 1; ++i) {
						temp.push_back(sortedUserTypeList[i].ptr);
					}
				}
				else {
					//std::cout << "no found" << "\n";
				}
			}

			return temp;
		}

		// deep copy.
		std::vector<UserType*> GetCopyUserTypeItem(const std::string& name) const { /// chk...
			std::vector<UserType*> temp;

			if (false == useSortedUserTypeList) {
				// make sortedUserTypeList.
				for (size_t i = 0; i < userTypeList.size(); ++i) {
					sortedUserTypeList.emplace_back(userTypeList[i], i);
				}

				std::sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

				useSortedUserTypeList = true;
			}
			// binary search
			{
				UserType x = UserType(name);
				bool err = false;
				size_t  idx = binary_find_ut(sortedUserTypeList, x, err);
				if (!err) {
					size_t start = idx;
					size_t last = idx;

					for (size_t i = idx; i > 0; --i) {
						if (name == sortedUserTypeList[i - 1].ptr->GetName()) {
							start--;
						}
						else {
							break;
						}
					}
					for (size_t i = idx + 1; i < sortedUserTypeList.size(); ++i) {
						if (name == sortedUserTypeList[i].ptr->GetName()) {
							last++;
						}
						else {
							break;
						}
					}

					for (size_t i = start; i < last + 1; ++i) {
						temp.push_back(new UserType(*sortedUserTypeList[i].ptr));
					}
				}
				else {
					//std::cout << "no found" << "\n";
				}
			}

			return temp;
		}

		std::vector<int> GetItemIdx(const std::string & name) const {
			std::vector<int> temp;
			/*if (String::startsWith(name, "$.") && name.size() >= 5) {
				// later, change to binary search?
				std::string str = name.substr(3, name.size() - 4);
				std::regex rgx(str);
				for (int i = 0; i < itemList.size(); ++i) {
					if (regex_match(itemList[i].GetName(), rgx)) {
						temp.push_back(itemList[i]);
					}
				}
			}
			else*/
			{
				if (false == useSortedItemList) {
					sortedItemList.clear();
					for (size_t i = 0; i < itemList.size(); ++i) {
						sortedItemList.push_back(WrapType<ItemType<std::string>*>((ItemType<std::string>*)&itemList[i], i));
					}

					std::stable_sort(sortedItemList.begin(), sortedItemList.end(), ItemTypeStringPtrCompare());

					useSortedItemList = true;
				}
				// binary search
				{
					bool err = false;
					ItemType<std::string> x = ItemType<std::string>(name, "");
					size_t  idx = binary_find_it(sortedItemList, x, err);

					if (!err) {
						size_t  start = idx;
						size_t  last = idx;

						for (size_t i = idx; i > 0; --i) {
							if (name == sortedItemList[i - 1].ptr->GetName()) {
								start--;
							}
							else {
								break;
							}
						}
						for (size_t i = idx + 1; i < sortedItemList.size(); ++i) {
							if (name == sortedItemList[i].ptr->GetName()) {
								last++;
							}
							else {
								break;
							}
						}

						for (size_t i = start; i < last + 1; ++i) {
							temp.push_back(sortedItemList[i].idx);
						}
					}
					else {
						//std::cout << "no found" << "\n";
					}
				}
			}
			return temp;
		}
	
				

	public:
		bool GetUserTypeItemRef(const size_t  idx, UserType*& ref)
		{
			ref = userTypeList[idx];
			return true;
		}
		bool GetLastUserTypeItemRef(UserType*& ref) {
			if (userTypeList.empty() == false) {
				ref = userTypeList.back();
				return true;
			}
			return false;
		}
	private:
		/// save1 - like EU4 savefiles.
		void Save1(std::ostream& stream, const UserType* ut, const int depth = 0) const {
			size_t  itemListCount = 0;
			size_t userTypeListCount = 0;

			const bool existUserType = ut->GetUserTypeListSize() > 0;

			for (size_t i = 0; i < ut->ilist.size(); ++i) {
				//std::cout << "ItemList" << endl;
				if (ut->ilist[i] == 1) {
					for (size_t j = 0; j < ut->itemList[itemListCount].size(); j++) {
						std::string temp;
						if (existUserType) {
							for (int k = 0; k < depth; ++k) {
								temp += "\t";
							}
						}
						if (ut->itemList[itemListCount].GetName() != "") {
							temp += ut->itemList[itemListCount].GetName();
							temp += " = ";
						}
						temp += ut->itemList[itemListCount].Get(j);
						if (j != ut->itemList[itemListCount].size() - 1) {
							temp += " ";
						}
						stream << temp;
					}
					if (i != ut->ilist.size() - 1) {
						if (existUserType) {
							stream << "\n";
						}
						else {
							stream << " "; // \n
						}
					}
					itemListCount++;
				}
				else if (ut->ilist[i] == 2) {
					// std::cout << "UserTypeList" << endl;
					for (int k = 0; k < depth; ++k) {
						stream << "\t";
					}

					if (ut->userTypeList[userTypeListCount]->GetName() != "") {
						stream << ut->userTypeList[userTypeListCount]->GetName() << " = ";
					}

					stream << "{\n";

					Save1(stream, ut->userTypeList[userTypeListCount], depth + 1);
					stream << "\n";

					for (int k = 0; k < depth; ++k) {
						stream << "\t";
					}
					stream << "}";
					if (i != ut->ilist.size() - 1) {
						stream << "\n";
					}

					userTypeListCount++;
				}
			}
		}
		/// save2 - test form.
		void Save2(std::ostream& stream, const UserType* ut, const int depth = 0) const {
			size_t  itemListCount = 0;
			size_t  userTypeListCount = 0;


			for (size_t i = 0; i < ut->ilist.size(); ++i) {
				//std::cout << "ItemList" << endl;
				if (ut->ilist[i] == 1) {
					for (size_t j = 0; j < ut->itemList[itemListCount].size(); j++) {
						//for (int k = 0; k < depth; ++k) {
						//	stream << "\t";
						//}
						if (ut->itemList[itemListCount].GetName() != "") {

							stream << ut->itemList[itemListCount].GetName() << " : ";
						}
						stream << ut->itemList[itemListCount].Get(j);
						if (j != ut->itemList[itemListCount].size() - 1) {
							stream << " ";
						}
					}

					if (i != ut->ilist.size() - 1) {
						stream << " ";//"\n";
					}
					itemListCount++;
				}
				else if (ut->ilist[i] == 2) {
					// std::cout << "UserTypeList" << endl;
					if (ut->userTypeList[userTypeListCount]->GetName() != "") {
						stream << ut->userTypeList[userTypeListCount]->GetName() << " ";
					}
					stream << "{\n";

					Save2(stream, ut->userTypeList[userTypeListCount], depth + 1);
					stream << "\n";

					for (int k = 0; k < depth; ++k) {
						stream << "\t";
					}
					stream << "}";
					if (i != ut->ilist.size() - 1) {
						stream << "\n";
					}
					userTypeListCount++;
				}
			}
		}

	public:
		void Save1(std::ostream& stream, int depth = 0) const {
			Save1(stream, this, depth);
		}

		void Save2(std::ostream& stream, int depth = 0) const {
			Save2(stream, this, depth);
		}

		std::string ItemListToString()const
		{
			std::string temp;
			size_t itemListCount = 0;

			for (size_t i = 0; i < itemList.size(); ++i) {
				for (size_t j = 0; j < itemList[itemListCount].size(); j++) {
					if (itemList[itemListCount].GetName() != "")
						temp = temp + itemList[itemListCount].GetName() + " = ";
					temp = temp + itemList[itemListCount].Get(j);
					if (j != itemList[itemListCount].size() - 1) {
						temp = temp + "/";
					}
				}
				if (i != itemList.size() - 1)
				{
					temp = temp + "/";
				}
				itemListCount++;
			}
			return temp;
		}
		std::string ItemListNamesToString()const
		{
			std::string temp;
			size_t  itemListCount = 0;

			for (size_t i = 0; i < itemList.size(); ++i) {
				for (size_t j = 0; j < itemList[itemListCount].size(); j++) {
					if (itemList[itemListCount].GetName() != "")
						temp = temp + itemList[itemListCount].GetName();
					else
						temp = temp + " ";

					if (j != itemList[itemListCount].size() - 1) {
						temp = temp + "/";
					}
				}
				if (i != itemList.size() - 1)
				{
					temp = temp + "/";
				}
				itemListCount++;
			}
			return temp;
		}
		std::vector<std::string> userTypeListNamesToStringArray()const
		{
			std::vector<std::string> temp;
			size_t userTypeListCount = 0;

			for (size_t i = 0; i < userTypeList.size(); ++i) {
				if (userTypeList[userTypeListCount]->GetName() != "") {
					temp.push_back(userTypeList[userTypeListCount]->GetName());
				}
				else {
					temp.push_back(" ");
				}
				userTypeListCount++;
			}
			return temp;
		}
		std::string UserTypeListNamesToString()const
		{
			std::string temp;
			size_t  userTypeListCount = 0;

			for (size_t i = 0; i < userTypeList.size(); ++i) {
				if (userTypeList[userTypeListCount]->GetName() != "") {
					temp = temp + userTypeList[userTypeListCount]->GetName();
				}
				else {
					temp = temp + " ";
				}

				if (i != itemList.size() - 1)
				{
					temp = temp + "/";
				}
				userTypeListCount++;
			}
			return temp;
		}
		std::string ToString()const
		{
			std::string temp;
			size_t itemListCount = 0;
			size_t  userTypeListCount = 0;

			for (size_t i = 0; i < ilist.size(); ++i) {
				//std::cout << "ItemList" << endl;
				if (ilist[i] == 1) {
					for (size_t j = 0; j < itemList[itemListCount].size(); j++) {
						if (itemList[itemListCount].GetName() != "") {
							temp.append(itemList[itemListCount].GetName());
							temp.append(" = ");
						}
						temp.append(itemList[itemListCount].Get(j));
						if (j != itemList[itemListCount].size() - 1)
						{
							temp.append(" ");
						}
					}
					if (i != ilist.size() - 1) {
						temp.append(" ");
					}
					itemListCount++;
				}
				else if (ilist[i] == 2) {
					// std::cout << "UserTypeList" << endl;
					if (userTypeList[userTypeListCount]->GetName() != "") {
						temp.append(userTypeList[userTypeListCount]->GetName());
						temp.append(" = ");
					}
					temp.append(" { ");
					temp.append(userTypeList[userTypeListCount]->ToString());
					temp.append(" ");
					temp.append(" }");
					if (i != ilist.size() - 1) {
						temp.append(" ");
					}

					userTypeListCount++;
				}
			}
			return temp;
		}
	public:
	};
