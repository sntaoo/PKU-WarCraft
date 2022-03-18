#include <assert.h>
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
vector<string> equipments = {"sword", "bomb", "arrow"};
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
    virtual void default_func(){};

    warrior(int id_, int strength_, string part_, string name_)
        : id(id_), strength(strength_), part(part_), name(name_) {}
    warrior(warrior& w) {
        id = w.get_id();
        strength = w.get_strength();
        part = w.get_part();
        name = w.get_name();
    }
};
typedef struct equipment {
    string name;
    equipment(string& name_) : name(name_) {}
} Eqp;
class dragon : public warrior {
   private:
    Eqp* eqp;
    double moral;

   public:
    string get_eqp_name() { return eqp->name; }
    double get_moral() { return moral; }
    dragon(warrior& w, string& eqp_name, double moral_)
        : warrior(w), moral(moral_) {
        eqp = new Eqp(eqp_name);
    }
};
class ninja : public warrior {
   private:
    Eqp *eqp1, *eqp2;

   public:
    string get_eqp_name() { return "a " + eqp1->name + " and a " + eqp2->name; }
    ninja(warrior& w, string& eqp_name1, string& eqp_name2) : warrior(w) {
        eqp1 = new Eqp(eqp_name1);
        eqp2 = new Eqp(eqp_name2);
    }
};
class iceman : public warrior {
   private:
    Eqp* eqp;

   public:
    string get_eqp_name() { return eqp->name; }
    iceman(warrior& w, string eqp_name) : warrior(w) {
        eqp = new Eqp(eqp_name);
    }
};
class lion : public warrior {
   private:
    int loyalty;

   public:
    int get_loyalty() { return loyalty; }
    lion(warrior& w, int loyalty_) : warrior(w), loyalty(loyalty_) {}
};
class wolf : public warrior {
   public:
    wolf(warrior& w) : warrior(w) {}
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
             << part_name_warrior_cnt[w.get_part()][w.get_name()] << " "
             << w.get_name() << " in " << w.get_part() << " headquarter\n";

        if (w.get_name() == "dragon") {
            dragon* p = dynamic_cast<dragon*>(&w);
            assert(p != nullptr);
            cout << "It has a " + p->get_eqp_name() << ",and it's morale is "
                 << fixed << setprecision(2) << p->get_moral() << endl;
        } else if (w.get_name() == "ninja") {
            ninja* p = dynamic_cast<ninja*>(&w);
            assert(p != nullptr);
            cout << "It has " << p->get_eqp_name() << endl;
        } else if (w.get_name() == "iceman") {
            iceman* p = dynamic_cast<iceman*>(&w);
            assert(p != nullptr);
            cout << "It has a " << p->get_eqp_name() << endl;
        } else if (w.get_name() == "lion") {
            lion* p = dynamic_cast<lion*>(&w);
            assert(p != nullptr);
            cout << "It's loyalty is " << p->get_loyalty() << endl;
        }
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
            part_warrior_cnt[part]++;
            part_name_warrior_cnt[part][warrior_build_order[part][ind]]++;
            cur_life_value -= warrior_strength_ready;
            increase_build_inds(part, 5);
            warrior* p = new warrior(warrior_id, warrior_strength_ready, part,
                                     warrior_build_order[part][ind]);
            ret = p;
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
                part_warrior_cnt[part]++;
                part_name_warrior_cnt[part]
                                     [warrior_build_order[part][approp_ind]]++;
                cur_life_value -=
                    warrior_strength[warrior_build_order[part][approp_ind]];
                increase_build_inds(part, 5);
                warrior* p = new warrior(
                    warrior_id,
                    warrior_strength[warrior_build_order[part][approp_ind]],
                    part, warrior_build_order[part][approp_ind]);
                ret = p;
            } else {
                this->stopped = true;
            }
        }
        return ret;
    }
    warrior* further_process(warrior* p) {
        if (p == nullptr) {
            return nullptr;
        }
        string name = p->get_name();
        warrior* ret = nullptr;
        if (name == "dragon") {
            double moral =
                (double)(cur_life_value) / (double)(p->get_strength());
            dragon* d = new dragon(*p, equipments[p->get_id() % 3], moral);
            ret = dynamic_cast<warrior*>(d);
        } else if (name == "ninja") {
            ninja* n = new ninja(*p, equipments[p->get_id() % 3],
                                 equipments[(p->get_id() + 1) % 3]);
            ret = dynamic_cast<warrior*>(n);
        } else if (name == "iceman") {
            iceman* i = new iceman(*p, equipments[p->get_id() % 3]);
            ret = dynamic_cast<warrior*>(i);
        } else if (name == "lion") {
            lion* l = new lion(*p, cur_life_value);
            ret = dynamic_cast<warrior*>(l);
        } else if (name == "wolf") {
            wolf* w = new wolf(*p);
            ret = dynamic_cast<warrior*>(w);
        }
        return ret;
    }
    void make_warriors() {
        if (!stopped) {
            warrior* p = further_process(make_warrior());
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
