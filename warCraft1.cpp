#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
unordered_map<string, unordered_map<string, int>>
    part_name_warrior_cnt;                    //映射某种战士的总数
unordered_map<string, int> part_warrior_cnt;  //映射某派战士数量
unordered_map<string, int> warrior_strength;  //映射某种战士的生命值
unordered_map<string, vector<string>>
    warrior_build_order;  //映射某派的战士制造顺序

vector<string> parts = {"red", "blue"};
vector<string> names = {"iceman", "lion", "wolf", "ninja", "dragon"};
class warrior {
   private:
    int id;        // 战士编号
    int strength;  // 生命值
    string part;   // 阵营
    string name;

   public:
    int get_id() { return warrior::id; }
    string& get_part() { return this->part; }
    string& get_name() { return this->name; }
    int get_strength() { return this->strength; }

    warrior(int id_, int strength_, string part_, string name_)
        : id(id_), strength(strength_), part(part_), name(name_) {}
};
class event_system {
   private:
    static int clock;

   public:
    static void happen(warrior& w) {
        cout << setw(3) << setfill('0') << event_system::clock << " "
             << w.get_part() << " " << w.get_name() << " "
             << part_warrior_cnt[w.get_part()] << " "
             << "born with strength " << w.get_strength() << ","
             << part_name_warrior_cnt[w.get_part()][w.get_name()] << " " << w.get_name() << " in "
             << w.get_part() << " headquarter\n";
    }
    static void happen(string part) {
        cout << setw(3) << setfill('0') << event_system::clock << " " << part
             << " headquarter stops making warriors\n";
    }
    static void set_clock(int val) { event_system::clock = val; }
    static void time_go() { event_system::clock++; }
};
int event_system::clock = 0;
class Base {
   private:
    string part;  //阵营
    int cur_life_value;
    static int red_build_ind;
    static int blue_build_ind;
    bool stopped;

   public:
    bool is_stopped() { return this->stopped; }
    static int get_red_build_ind() { return Base::red_build_ind; }
    static int get_blue_build_ind() { return Base::blue_build_ind; }
    static int increase_build_inds(string part, int bound) {
        // todo
        if ("red" == part) {
            Base::red_build_ind = (Base::red_build_ind + 1) % bound;
            return Base::red_build_ind;
        } else {
            Base::blue_build_ind = (Base::blue_build_ind + 1) % bound;
            return Base::blue_build_ind;
        }
    }
    static void set_red_build_ind(int val) { Base::red_build_ind = val; }
    static void set_blue_build_ind(int val) { Base::blue_build_ind = val; }
    Base(string part_, int cur_life_value_)
        : part(part_), cur_life_value(cur_life_value_) {
        this->stopped = false;
    }
    warrior* make_warrior() {
        warrior* ret = nullptr;
        int warrior_id = part_warrior_cnt[part] + 1;
        int ind = -1;
        ind = part == "red" ? red_build_ind : blue_build_ind;
        // todo
        // assert(ind != -1);
        int warrior_strength_ready =
            warrior_strength[warrior_build_order[part][ind]];
        if (warrior_strength_ready <= cur_life_value) {
            warrior* p = new warrior(warrior_id, warrior_strength_ready, part,
                                     warrior_build_order[part][ind]);
            part_warrior_cnt[part]++;
            part_name_warrior_cnt[part][warrior_build_order[part][ind]]++;
            ret = p;
            cur_life_value -= warrior_strength_ready;
            increase_build_inds(part, 5);
        } else {
            int end_ind = ind;
            int approp_ind;
            bool found = false;
            do {
                approp_ind = increase_build_inds(part, 5);
                if (warrior_strength[warrior_build_order[part][approp_ind]] <=
                    cur_life_value) {
                    found = true;
                    break;
                }

            } while (approp_ind != end_ind);
            if (found) {
                warrior* p = new warrior(
                    warrior_id,
                    warrior_strength[warrior_build_order[part][approp_ind]],
                    part, warrior_build_order[part][approp_ind]);
                part_warrior_cnt[part]++;
                part_name_warrior_cnt[part][warrior_build_order[part][approp_ind]]++;
                ret = p;
                cur_life_value -=
                    warrior_strength[warrior_build_order[part][approp_ind]];
                increase_build_inds(part, 5);
            } else {
                this->stopped = true;
            }
        }
        return ret;
    }
    void make_warriors() {
        if (!stopped) {
            warrior* p = make_warrior();
            if (p == nullptr) {
                event_system::happen(this->part);
            } else {
                event_system::happen(*p);
            }
        }
    }
};

int Base::blue_build_ind = 0;
int Base::red_build_ind = 0;
void __Init__() {
    event_system::set_clock(0);
    Base::set_blue_build_ind(0);
    Base::set_red_build_ind(0);
    vector<string> red_order = {"iceman", "lion", "wolf", "ninja", "dragon"};
    vector<string> blue_order = {"lion", "dragon", "ninja", "iceman", "wolf"};
    warrior_build_order.insert(pair<string, vector<string>>("red", red_order));
    warrior_build_order.insert(
        pair<string, vector<string>>("blue", blue_order));
    for (string part : parts) {
        part_warrior_cnt[part] = 0;
        for (string name : names) {
            part_name_warrior_cnt[part][name] = 0;
        }
    }

}
int main() {
    int tot;
    scanf("%d", &tot);
    for (int c = 1; c <= tot; c++) {
        cout << "Case:" << c << endl;
        __Init__();
        int cur_life_value;
        cin >> cur_life_value;
        cin >> warrior_strength["dragon"] >> warrior_strength["ninja"] >>
            warrior_strength["iceman"] >> warrior_strength["lion"] >>
            warrior_strength["wolf"];
        Base red_base("red", cur_life_value);
        Base blue_base("blue", cur_life_value);
        while (!red_base.is_stopped() || !blue_base.is_stopped()) {
            red_base.make_warriors();
            blue_base.make_warriors();
            event_system::time_go();
        }
    }
}
