#include <iostream>
#include <list>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <ctime>

#include <fcntl.h>
#include <io.h>

using namespace std;

const size_t NUM_ORG_PILES = 4;
const size_t NUM_PILES = 7;
const size_t NUM_SUITS = 4;
const size_t NUM_VALUES = 13;
const char SUITS[NUM_SUITS] = { 'C', 'D', 'H', 'S' };
const char VALUES[NUM_VALUES] = { 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', };

class card {
private:
	bool show;
	char suit;
	char value;

public:
	int get_real_value() {
		char value = this->get_value();
		switch (value)
		{
		case 'A':
			return 1;
		case 'T':
			return 10;
		case 'J':
			return 11;
		case 'Q':
			return 12;
		case 'K':
			return 13;
		default:
			int result = (int)(value - '0');

			if (result >= 0 && result <= 9) {
				return result;
			}
			else {
				return -1;
			}
		}
	}
	bool get_show() {
		return this->show;
	}
	char get_suit() {
		return this->suit;
	}
	char get_value() {
		return this->value;
	}
	void set_show(bool show) {
		this->show = show;
	}
	void set_suit(char suit) {
		this->suit = suit;
	}
	void set_value(char value) {
		this->value = value;
	}
	bool is_suit_black() {
		suit = this->get_suit();
		if (suit == 'C' || suit == 'S') {
			return true;
		}
		else {
			return false;
		}
	}
	bool invalid_card() {
		stringstream ss;
		if (this->get_show() == true) {
			ss << this->get_value();
			ss << this->get_suit();
		}
		else {
			ss << '?' << '?';
		}

		if (ss.str() == "XX") {
			return true;
		}
		else {
			return false;
		}
	}
	string get_card() {
		stringstream ss;
		if (this->get_show() == true) {
			ss << this->get_value();
			ss << this->get_suit();
		}
		else {
			ss << '?' << '?';
		}

		return ss.str();
	}
	wstring get_card_out() {
		wstring ws;
		if (this->get_show() == true) {
			if (this->is_suit_black() == true) {
				ws = L"\033[1;37m";
			}
			else if (this->get_suit() != 'X') {
				ws = L"\033[1;31m";
			} 
			else {
				ws = L"\033[1;37m";
			}
			char value = this->get_value();
			if (value == 'X') {
				ws += L'\u005B';
			}
			else {
				ws += this->get_value();
			}
			char suit = this->get_suit();
			if (suit == 'C') {
				ws += L'\u2663';
			}
			else if (suit == 'D') {
				ws += L'\u2666';
			}
			else if (suit == 'H') {
				ws += L'\u2665';
			}
			else if (suit == 'S') {
				ws += L'\u2660';
			}
			else {
				ws += L'\u005D';
			}
			ws += L"\033[0m";
		}
		else {
			ws = L"\033[1;37m";
			ws += L'\u039E';
			ws += L'\u039E';
			ws += L"\033[0m";
		}

		return ws;
	}
	card() {
		this->set_show(true);
		this->set_suit('X');
		this->set_value('X');
	}
	card(bool show, char suit, char value) {
		this->set_show(show);
		this->set_suit(suit);
		this->set_value(value);
	}
};

class board {
private:
	bool running, restart, win;
	card org_piles[NUM_ORG_PILES];
	card waste;
	list<card> piles_stacks[NUM_PILES];
	list<card> stock_queue;
public:
	bool is_running() {
		return this->running;
	}
	bool get_restart() {
		return this->restart;
	}
	bool get_win() {
		return this->win;
	}
	card get_org_piles_card(size_t index) {
		return this->org_piles[index];
	}
	card get_waste_card(bool remove) {
		card c = this->waste;
		if (remove == true) {
			this->set_waste_card(card());
		}
		return c;
	}
	list<card> get_piles_stacks_cards(size_t index, size_t card_qnty, bool remove) {
		list<card> cards;
		list<card>::iterator it;

		if (this->piles_stacks[index].empty() == true) {
			return cards;
		}

		it = this->piles_stacks[index].begin();

		advance(it, this->piles_stacks[index].size() - card_qnty);
		cards.push_back(it._Ptr->_Myval);

		for (size_t x = 0; x < card_qnty - 1; x++) {
			it++;
			cards.push_back(it._Ptr->_Myval);
		}

		if (remove == true && this->piles_stacks[index].size() >= card_qnty) {
			for (size_t x = 0; x < card_qnty; x++) {
				this->piles_stacks[index].pop_back();
			}
		}

		return cards;
	}
	card get_stock_queue_card(bool remove) {
		card c;

		if (this->stock_queue.empty() == true) {
			c = card();
		}
		else {
			c = this->stock_queue.front();
			if (remove == true) {
				this->stock_queue.pop_front();
			}
		}
		
		return c;
	}
	void set_running(bool running) {
		this->running = running;
	}
	void set_restart(bool restart) {
		this->restart = restart;
	}
	void set_win(bool win) {
		this->win = win;
	}
	void set_org_piles_card(size_t index, card new_card) {
		this->org_piles[index] = new_card;
	}
	void set_waste_card(card new_card) {
		new_card.set_show(true);
		this->waste = new_card;
	}
	void set_piles_stacks_card(size_t index, list<card> new_cards) {
		list<card>::iterator it;

		for (it = new_cards.begin(); it != new_cards.end(); it++) {
			this->piles_stacks[index].push_back(it._Ptr->_Myval);
		}
	}
	void set_stock_queue_card(card new_card) {
		new_card.set_show(false);
		this->stock_queue.push_back(new_card);
	}
	bool legal_move_org(card src_card, size_t target_index) {
		if ((this->get_org_piles_card(target_index).invalid_card() == true && src_card.get_value() == 'A') ||
			(src_card.get_real_value() - this->get_org_piles_card(target_index).get_real_value() == 1 &&
				src_card.get_suit() == this->get_org_piles_card(target_index).get_suit())) {
			return true;
		}

		return false;
	}
	bool legal_move_pile(card src_card, size_t target_index) {
		if (this->piles_stacks[target_index].empty() == true && src_card.invalid_card() == false) {
			return true;
		}
		else {
			list<card> cards = this->get_piles_stacks_cards(target_index, 1, false);

			if (cards.empty()) {
				return false;
			}
			card target_card = cards.back();

			if (src_card.is_suit_black() != target_card.is_suit_black()) {
				if (src_card.get_real_value() + 1 == target_card.get_real_value()) {
					return true;
				}
			}
		}

		return false;
	}
	bool waste_pile_possible() {
		card src = this->get_waste_card(false);
		for (size_t x = 0; x < NUM_PILES; x++) {
			if (this->legal_move_pile(src, x)) {
				return true;
			}
		}
		return false;
	}
	bool waste_org_possible() {
		card src = this->get_waste_card(false);
		for (size_t x = 0; x < NUM_ORG_PILES; x++) {
			if (this->legal_move_org(src, x)) {
				return true;
			}
		}
		return false;
	}
	bool pile_org_possible() {
		for (size_t x = 0; x < NUM_PILES; x++) {
			list<card> src_l = this->get_piles_stacks_cards(x, 1, false);
			if (src_l.empty() == false) {
				card src = src_l.front();
				for (size_t y = 0; y < NUM_ORG_PILES; y++) {
					if (this->legal_move_org(src, y) && src.get_show() == true) {
						return true;
					}
				}
			}
		}
		return false;
	}
	bool pile_pile_possible() {
		for (size_t x = 0; x < NUM_PILES; x++) {
			for (size_t y = 0; y < this->piles_stacks[x].size(); y++) {
				list<card> src_l = this->get_piles_stacks_cards(x, y + 1, false);
				if (src_l.empty() == false) {
					card src = src_l.front();
					for (size_t z = 0; z < NUM_PILES; z++) {
						if (this->legal_move_pile(src, z) && src.get_show() == true) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	bool check_win() {
		for (size_t x = 0; x < NUM_ORG_PILES; x++) {
			if (this->get_org_piles_card(x).get_value() != 'K') {
				return false;
			}
		}

		return true;
	}
	bool check_end_game() {
		if (!this->pile_pile_possible()) {
			if (!this->pile_org_possible()) {
				if (!this->waste_pile_possible()) {
					if (!this->waste_org_possible()) {
						size_t len_stock = this->stock_queue.size();
						if (len_stock == 1 && this->get_stock_queue_card(false).invalid_card() == true) {
							if (check_win() == true) {
								this->set_win(true);
							}
							return true;
						}
						else {
							list<card>::iterator it;
							for (it = this->stock_queue.begin(); it != this->stock_queue.end(); it++) {
								for (size_t y = 0; y < NUM_PILES; y++) {
									if (this->legal_move_pile(it._Ptr->_Myval, y)) {
										return false;
									}
								}
								for (size_t y = 0; y < NUM_ORG_PILES; y++) {
									if (this->legal_move_org(it._Ptr->_Myval, y)) {
										return false;
									}
								}
							}
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	void move_pile_org(size_t src_index, size_t target_index) {
		list<card> src_cards = this->get_piles_stacks_cards(src_index, 1, true);
		if (src_cards.empty()) {
			wcout << L"\nIllegal Move";
			return;
		}
		card src_card = src_cards.back();
		card target_card = this->get_org_piles_card(target_index);

		if (this->legal_move_org(src_card, target_index)) {
			this->set_org_piles_card(target_index, src_card);
			if (this->piles_stacks[src_index].empty() == false) {
				this->piles_stacks[src_index].back().set_show(true);
			}
		}
		else {
			this->set_piles_stacks_card(src_index, src_cards);
			wcout << L"\nIllegal Move";
		}
	}
	void move_pile_pile(size_t src_index, size_t target_index, size_t card_qnty) {
		bool illegal_move = false;
		size_t length = this->piles_stacks[src_index].size();
		list<card> src_cards;

		if (length >= card_qnty) {
			card actual, next;
			list<card>::iterator actual_it, next_it;
			src_cards = this->get_piles_stacks_cards(src_index, card_qnty, true);

			if (src_cards.empty() == false) {
				next_it = src_cards.begin();

				for (actual_it = src_cards.begin(); actual_it._Ptr->_Myval.get_card() != src_cards.back().get_card(); actual_it++) {
					next_it++;
					actual = actual_it._Ptr->_Myval;
					next = next_it._Ptr->_Myval;
					if (actual.get_real_value() - next.get_real_value() != 1 || actual.is_suit_black() == next.is_suit_black()) {
						illegal_move = true;
						break;
					}
				}

				if (illegal_move == false) {
					if (legal_move_pile(src_cards.front(), target_index)) {
						this->set_piles_stacks_card(target_index, src_cards);
						if (this->piles_stacks[src_index].empty() == false) {
							this->piles_stacks[src_index].back().set_show(true);
						}

						return;
					}
					else {
						wcout << L"\nIllegal Move";
					}
				}
				else {
					wcout << L"\nIllegal Move";
				}

				this->set_piles_stacks_card(src_index, src_cards);
			}
			else {
				wcout << L"\nIllegal Move";
			}
		}
		else {
			wcout << L"\nIllegal Move";
		}
	}
	void move_waste_org(size_t target_index) {
		card waste_card = this->get_waste_card(true);
		card target_card = this->get_org_piles_card(target_index);

		if ((target_card.invalid_card() == true && waste_card.get_value() == 'A') ||
			(waste_card.get_real_value() - target_card.get_real_value() == 1 && waste_card.get_suit() == target_card.get_suit())) {
			this->set_org_piles_card(target_index, waste_card);
			this->set_waste_card(this->get_stock_queue_card(true));
		}
		else {
			this->set_waste_card(waste_card);
			wcout << L"\nIllegal Move";
		}
	}
	void move_waste_pile(size_t target_index) {
		list<card> cards;
		card waste_card = this->get_waste_card(true);
		
		cards.push_back(waste_card);

		if (legal_move_pile(waste_card, target_index)) {
			this->set_piles_stacks_card(target_index, cards);
			this->set_waste_card(this->get_stock_queue_card(true));
		}
		else {
			this->set_waste_card(waste_card);
			wcout << L"\nIllegal Move";
		}
	}
	void print_piles() {
		size_t bigger = 0;
		list<card>::iterator it[NUM_PILES];

		for (size_t x = 0; x < NUM_PILES; x++) {
			if (this->piles_stacks[x].empty() == true) {
				wcout << L"XX" << '\t';
			}
			else {
				if (bigger < this->piles_stacks[x].size() - 1) {
					bigger = this->piles_stacks[x].size() - 1;
				}
				it[x] = this->piles_stacks[x].begin();
				wcout << it[x]._Ptr->_Myval.get_card_out() << '\t';
			}
		}

		wcout << '\n';

		for (size_t x = 0; x < bigger; x++) {
			for (size_t y = 0; y < NUM_PILES; y++) {
				if (this->piles_stacks[y].size() > x+1) {
					it[y]++;
					wcout << it[y]._Ptr->_Myval.get_card_out() << '\t';
				}
				else {
					wcout << '\t';
				}
			}

			wcout << '\n';
		}
	}
	void display() {
		size_t action;
		size_t src_pile, target_pile, card_qnty;

		wcout << this->get_stock_queue_card(false).get_card_out() << '\t';

		wcout << this->get_waste_card(false).get_card_out() << '\t' << '\t';
		
		for (size_t x = 0; x < NUM_ORG_PILES; x++) {
			wcout << this->get_org_piles_card(x).get_card_out() << '\t';
		}

		wcout << '\n' << '\n';

		this->print_piles();

		wcout << '\n' << '\n';
		
		if (this->check_end_game() == true) {
			if (this->get_win() == true) {
				wcout << L"YOU WON!!!\n\n";
			}
			else {
				wcout << L"GAME OVER\n\n";
			}
			wcout << L"Restart?\n";
			wcout << L"1 - Yes\n";
			wcout << L"2 - No\n";

			cin >> action;

			if (cin.good()) {
				switch (action) {
				case 1: {
					this->set_restart(true);
					break;
				}
				case 2: {
					this->set_running(false);
					break;
				}
				default: {
					wcout << L"\nInvalid option";
					break;
				}
				}
			}
			else {
				wcout << L"\nInvalid option";
			}
		}
		else {
			wcout << L"Type the number for the action:\n";
			wcout << L"1 - Draw a card\n";
			wcout << L"2 - Move from a pile to another pile\n";
			wcout << L"3 - Move from a pile to a pile of organization\n";
			wcout << L"4 - Move from waste pile to a pile\n";
			wcout << L"5 - Move from waste pile to a pile of organization\n";
			wcout << L"6 - Restart\n";
			wcout << L"7 - Exit\n\n";

			cin >> action;

			if (cin.good()) {
				switch (action) {
				case 1: {
					card stock_card = this->get_stock_queue_card(true);
					card waste_card;
					if (stock_card.invalid_card() == false) {
						waste_card = this->get_waste_card(true);
						this->set_waste_card(stock_card);
						if (waste_card.invalid_card() == false) {
							this->set_stock_queue_card(waste_card);
						}
					}
					else {
						wcout << L"\nIllegal Move";
					}

					break;
				}
				case 2: {
					wcout << L"\nSource pile: ";
					cin >> src_pile;

					if (cin.good()) {
						wcout << L"\nTarget pile: ";
						cin >> target_pile;

						if (cin.good()) {
							wcout << L"\nCard quantity: ";
							cin >> card_qnty;

							if (cin.good()) {
								if (src_pile >= 1 && src_pile <= NUM_PILES && target_pile >= 1 && target_pile <= NUM_PILES) {
									this->move_pile_pile(src_pile - 1, target_pile - 1, card_qnty);
								}
								else {
									wcout << L"\nIllegal Move";
								}
							}
							else {
								wcout << L"\nIllegal Move";
							}
						}
						else {
							wcout << L"\nIllegal Move";
						}
					}
					else {
						wcout << L"\nIllegal Move";
					}

					break;
				}
				case 3: {
					wcout << L"\nSource pile: ";
					cin >> src_pile;

					if (cin.good()) {
						wcout << L"\nTarget pile of organization: ";
						cin >> target_pile;

						if (cin.good()) {
							if (src_pile >= 1 && src_pile <= NUM_PILES && target_pile >= 1 && target_pile <= NUM_ORG_PILES) {
								this->move_pile_org(src_pile - 1, target_pile - 1);
							}
							else {
								wcout << L"\nIllegal Move";
							}
						}
					}
					else {
						wcout << L"\nIllegal Move";
					}

					break;
				}
				case 4: {
					wcout << L"\nTarget pile: ";
					cin >> target_pile;

					if (cin.good()) {
						if (target_pile >= 1 && target_pile <= NUM_PILES) {
							this->move_waste_pile(target_pile - 1);
						}
						else {
							wcout << L"\nIllegal Move";
						}
					}
					else {
						wcout << L"\nIllegal Move";
					}

					break;
				}
				case 5: {
					wcout << L"\nTarget pile of organization: ";
					cin >> target_pile;

					if (cin.good()) {
						if (target_pile >= 1 && target_pile <= NUM_ORG_PILES) {
							this->move_waste_org(target_pile - 1);
						}
						else {
							wcout << L"\nIllegal Move";
						}
					}
					else {
						wcout << L"\nIllegal Move";
					}

					break;
				}
				case 6: {
					this->set_restart(true);

					break;
				}
				case 7: {
					this->set_running(false);

					break;
				}
				default:
					wcout << L"\nInvalid option";
					break;
				}
			}
			else {
				wcout << L"\nInvalid option";
			}
			cin.clear();
			cin.ignore();
		}
	}
	board() {
		size_t index;
		list<card> aux;
		list<card> deck;
		list<card>::iterator it;

		this->set_running(true);
		this->set_restart(false);
		this->set_win(false);

		for (size_t x = 0; x < NUM_SUITS; x++) {
			for (size_t y = 0; y < NUM_VALUES; y++) {
				deck.push_back(card(false, SUITS[x], VALUES[y]));
			}
		}
		
		for (size_t x = 0; x < NUM_PILES; x++) {
			for (size_t y = 0; y < x + 1; y++) {
				srand((unsigned int)time(0));
				index = rand() % deck.size();

				it = deck.begin();
				advance(it, index);

				aux.push_back(it._Ptr->_Myval);

				this->set_piles_stacks_card(x, aux);

				aux.pop_back();

				deck.erase(it);

				if (y == x) {
					this->piles_stacks[x].back().set_show(true);
				}
			}
		}

		while (deck.size() > 0) {
			index = rand() % deck.size();

			it = deck.begin();
			advance(it, index);

			this->set_stock_queue_card(it._Ptr->_Myval);

			deck.erase(it);
		}
	}
};

// STOCK -> QUEUE
// WASTE -> ELEMENT
// ORG_PILES -> ELEMENT[4]
// PILES -> STACK[7]
// main() is where program execution begins.
int main() {
	card c;
	board my_board = board();

	int result = _setmode(_fileno(stdout), _O_WTEXT);

	while (my_board.is_running()) {
		my_board.display();
		wcout << wstring(9, '\n');

		if (my_board.get_restart()) {
			my_board = board();
		}
	}

	return 0;
}
