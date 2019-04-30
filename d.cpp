#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>

int Count(int* a, int* b, int n) {
	int sum = 0;

	for (int i = 0; i < n; ++i) {
		if (a[i] != b[i]) {
			++sum;
		}
	}

	return sum;
}

typedef std::vector<std::vector<int>> Matrix;


bool _bfs(const std::vector<std::vector<int>>& _list, int start, int last, std::queue<int>& S,
	std::vector<bool>& found,
	std::vector<int>& parent, int n) {



	S.push(start);
	while (!S.empty()) {
		int v = S.front();
		S.pop();

		if (v == last) {
			return true;
		}

		for (int i = 0; i < _list[v].size(); ++i) {
			int w = _list[v][i];

			if (!found[w]) {
				found[w] = true;
				parent[w] = v;
				S.push(w);
			}
		}
	}
	return false;
}
bool bfs(const std::vector<std::vector<int>>& _list, int start, int last, int n) {
	std::queue<int> S;
	std::vector<bool> found(n, false);
	std::vector<int> parent(n, -1);

	if (_bfs(_list, start, last, S, found, parent, n)) {
		std::vector<int> _stack;

		_stack.push_back(last);
		while (last != -1 && last != start) {
			_stack.push_back(parent[last]);
			last = parent[last];
		}

		while (!_stack.empty()) {
			std::cout << (_stack.back() + 1) << " ";
			_stack.pop_back();

		}

		return true;
	}
	return false;
}

int main(void) 
{
	int n, k;

	std::cin >> n >> k;
	
	int arr[1000][30];
	int dist[1000]; // (n, std::vector<int>(n, 10000));
	std::vector<int> prev(1000, -1); // Matrix next(n, std::vector<int>(n, -1));

	std::vector<std::multimap<int, int>> vec_of_map(k + 1, std::multimap<int,int>());

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < k; ++j) {
			char x;
			std::cin >> x;
			arr[i][j] = x - '0';
		}
	}

	int start, last;

	std::cin >> start >> last;

	start = start - 1;
	last = last - 1;

	

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (i != j) {
				int x = Count(arr[i], arr[j], k);
				vec_of_map[x].insert(std::make_pair(i, j));
				vec_of_map[x].insert(std::make_pair(j, i));
			}
		}
	}

	bool pass = false;
	for (int i = 0; i < k; ++i) {
		if (vec_of_map[i].find(start) != vec_of_map[i].end()
			&& vec_of_map[i].find(last) != vec_of_map[i].end()) {

			std::vector<std::vector<int>> _list(n);

			for (auto x = vec_of_map[i].begin(); x != vec_of_map[i].end(); ++x) {
				_list[x->first].push_back(x->second);
			}
			
			if (bfs(_list, start, last, n)) {
				pass = true;
				break;
			}
		}
	}		
	if (!pass) {
		std::cout << -1;
	}

	return 0;
}
