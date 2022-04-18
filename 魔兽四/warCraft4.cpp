#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
class Base;
class city;
enum flag { NO_FLAG, RED_FLAG, BLUE_FLAG };
enum fight_result { BOTH_DEAD, BOTH_ALIVE, BLUE_WIN, RED_WIN, NO_WAR };
class warrior;
vector<string> weapon_kinds = {"sword", "bomb", "arrow"};
unordered_map<string, int> part_warrior_cnt;  //映射某派战士数量,用于生成id
unordered_map<string, int>
    warrior_element;  //映射某种战士的生命值，用于保存输入数据
unordered_map<string, int> warrior_force;  //映射武士攻击力
unordered_map<string, vector<string>>
    warrior_build_order;  //映射某派的战士制造顺序
vector<city> cities;      //城市向量
vector<string> red_order = {"iceman", "lion", "wolf", "ninja", "dragon"};
vector<string> blue_order = {"lion", "dragon", "ninja", "iceman", "wolf"};
unordered_map<string, Base*> part2base;  // 通过名字映射基地类指针
int loyalty_decrease, time_limit, city_count, arrow_force;
class weapon {
   public:
    string name;
    weapon(string name_) : name(name_) {}
    virtual bool is_usable();
    virtual void attack(warrior* attacker, warrior* injure);
};
class sword : public weapon {
   public:
    int force;
    sword(string name_, double force_) : weapon(name_), force(force_) {}
    void blunt();
    virtual bool is_usable();
    virtual void attack(warrior* attacker, warrior* injure);
};
class arrow : public weapon {
   public:
    int use_count;
    int arrow_force;
    arrow(string name_, int arrow_force_)
        : weapon(name_), use_count(0), arrow_force(arrow_force_) {}
    virtual bool is_usable();
    virtual void attack(warrior* attacker, warrior* injure);
};
class bomb : public weapon {
   public:
    bool is_new;
    virtual bool is_usable();
    bomb(string name_) : weapon(name_), is_new(true) {}
    virtual void attack(warrior* attacker, warrior* injure);
};
class warrior {
   public:
    int id;        // 战士编号
    int elements;  // 生命值
    int force;     //攻击力
    string part;   // 阵营
    string name;   //名称
    int location;  //所在城市
    unordered_map<string, weapon*> weapons;
    warrior(int id_,
            int elements_,
            string part_,
            string name_,
            int location_,
            int force_)
        : id(id_),
          elements(elements_),
          part(part_),
          name(name_),
          location(location_),
          force(force_) {
        for (string& weapon_name : weapon_kinds)
            weapons[weapon_name] = nullptr;
    }
    warrior(warrior& w) {
        id = w.id;
        elements = w.elements;
        force = w.force;
        part = w.part;
        name = w.name;
        location = w.location;
        for (string& weapon_name : weapon_kinds)
            weapons[weapon_name] = w.weapons[weapon_name];
        for (string& weapon_name : weapon_kinds)
            w.weapons[weapon_name] = nullptr;
    }

    virtual void march();
    void clear_weapons();
    void reach_headquarter();

    bool is_alive();

    virtual void action_win();  //龙士气增加，狼抢夺武器

    virtual void action_NOT_win();  //龙士气减少，狮子忠诚度减少

    //攻击敌人
    void attack(warrior* enemy);

    //反击
    virtual void fight_back(warrior* enemy);

    void use_sword(warrior* enemy);  // 返回宝剑攻击力并使宝剑钝化
    int get_sword_force();
    bool check_if_use_bomb(
        warrior* enemy);  //如果没有炸弹，或者至少有一方已死亡一定返回false
    virtual ~warrior() {
        if (weapons["bomb"] != nullptr)
            delete weapons["bomb"];
        if (weapons["arrow"] != nullptr)
            delete weapons["arrow"];
        if (weapons["sword"] != nullptr)
            delete weapons["sword"];
    }
};
class dragon : public warrior {
   public:
    double morale;
    void yell();  //存活的龙才能欢呼
    virtual void action_win();
    virtual void action_NOT_win();
    dragon(warrior& w, double morale_) : warrior(w), morale(morale_) {}
};
class lion : public warrior {
   public:
    int loyalty;
    bool escape();
    virtual void action_NOT_win();
    lion(warrior& w, int loyalty_) : warrior(w), loyalty(loyalty_) {}
};
class ninja : public warrior {
   public:
    ninja(warrior& w) : warrior(w) {}
    virtual void fight_back(warrior* w);  // ninja挨打从不反击
};
class iceman : public warrior {
   public:
    int steps;             // 至今走了多少步
    virtual void march();  // 每两步血量降低且攻击增加
    void element_decrease();
    iceman(warrior& w) : warrior(w), steps(0) {}
};
class wolf : public warrior {
   public:
    wolf(warrior& w) : warrior(w) {}
    virtual void action_win();  // 抢夺敌人武器
};
class city {
   public:
    warrior* first;
    warrior* second;
    int location;
    flag flag_status;
    int elements;
    fight_result previous_fight_result;
    void generate_elements();  //生成生命元
    void raise_flag(flag flag_to_raise);
    city(warrior* first_, warrior* second_, int location_)
        : first(first_),
          second(second_),
          flag_status(NO_FLAG),
          elements(0),
          previous_fight_result(NO_WAR),
          location(location_) {}
    warrior*
    who_attack_first();  //决定谁先手,此处仅为理论上结果，不管存活与否,是否为空
    fight_result check_fight_result();
    ~city() {
        if (first != nullptr)
            delete first;
        if (second != nullptr)
            delete second;
    }
};
class Base {
   public:
    string part;         //阵营
    int cur_life_value;  //当前剩余生命值
    int enemy_count;
    warrior* first_arrived_enemy;
    static int red_build_ind;   //制造第几个红战士
    static int blue_build_ind;  //制造第几个蓝战士
    static void increase_build_inds(string part, int bound);
    Base(string part_, int cur_life_value_)
        : part(part_),
          cur_life_value(cur_life_value_),
          enemy_count(0),
          first_arrived_enemy(nullptr) {}

    warrior* make_warrior();
    //制造武士时为其分配武器
    weapon* make_weapon(warrior* owner, string weapon_name);
    warrior* further_process(warrior* p);
    void make_warriors();
    void award_warrior(warrior* w);
};
//事件系统，统筹战斗的所有时间
class event_system {
   public:
    static int clock;
    static bool conqured_red, conqured_blue;
    static void happen00_warrior_born(Base& red, Base& blue);
    static void warrior_born(warrior& w);
    static void time_go();
    static string get_format_clock();
    static void happen05_lion_escape();
    static void happen10_warrior_march();
    static void happen20_generate_elements();
    static void happen30_take_city_elements();
    static void happen35_shot_arrow();
    static void happen38_use_bomb();
    static fight_result fight(warrior* r, warrior* b);
    static void take_actions_after_war(city& c);  // 增减士气、忠诚度，抢夺武器
    static vector<int> get_city_elements_after_war(city& c);
    static void raise_flag_after_war(city& c);
    static void change_prev_fight_result(city& c);
    static void happen40_war();
    static void happen50_report_base(Base& red, Base& blue);
    static void report_base(Base& base);
    static void report_warrior(warrior* w);
    static void happen55_report_warriors();
};
ostream& operator<<(ostream& out, warrior& w) {
    out << w.part << " " << w.name << " " << w.id;
    return out;
}
ostream& operator<<(ostream& out, sword* sw) {
    if (sw != nullptr)
        out << "sword(" << sw->force << ")";
    return out;
}
ostream& operator<<(ostream& out, bomb* bo) {
    if (bo != nullptr)
        out << "bomb";
    return out;
}
ostream& operator<<(ostream& out, arrow* ar) {
    if (ar != nullptr)
        out << "arrow(" << (3 - ar->use_count) << ")";
    return out;
}
bool weapon::is_usable() {
    return false;
}
void weapon::attack(warrior* attacker, warrior* injure) {
    return;
}

bool sword::is_usable() {
    return force > 0;
}
void sword::blunt() {
    force = (force * 8) / 10;
}
void sword::attack(warrior* attacker, warrior* injure) {
    return;
}

bool arrow::is_usable() {
    return use_count < 3;
}
void arrow::attack(warrior* attacker, warrior* injure) {
    if (!is_usable())
        return;
    use_count++;
    injure->elements -= arrow_force;
}

bool bomb::is_usable() {
    return is_new;
}
void bomb::attack(warrior* attacker, warrior* injure) {
    is_new = false;
    attacker->elements = 0;
    injure->elements = 0;
}

void warrior::march() {
    int direction = part == "red" ? 1 : -1;  //红军为1向右走，-1向左走
    location += direction;
    cout << event_system::get_format_clock() << " " << *this
         << " marched to city " << location << " with " << elements
         << " elements and force " << force << endl;
}
void warrior::clear_weapons() {
    for (string& weapon_name : weapon_kinds) {
        if (weapons[weapon_name] != nullptr &&
            !weapons[weapon_name]->is_usable()) {
            delete weapons[weapon_name];
            weapons[weapon_name] = nullptr;
        }
    }
}
void warrior::reach_headquarter() {
    if (name == "iceman") {
        iceman* i = dynamic_cast<iceman*>(this);
        i->steps++;
        i->element_decrease();
    }
    string enemy_part = part == "blue" ? "red" : "blue";
    cout << event_system::get_format_clock() << " " << *this << " reached "
         << enemy_part << " headquarter with " << elements
         << " elements and force " << force << endl;
    part2base[enemy_part]->enemy_count++;
    if (part2base[enemy_part]->first_arrived_enemy == nullptr)
        part2base[enemy_part]->first_arrived_enemy = this;
    if (part2base[enemy_part]->enemy_count >= 2) {
        cout << event_system::get_format_clock() << " " << enemy_part
             << " headquarter was taken\n";
        if (enemy_part == "red")
            event_system::conqured_red = true;
        else
            event_system::conqured_blue = true;
    }
}
bool warrior::is_alive() {
    return elements > 0;
}
void warrior::action_win() {
    return;
}
void warrior::action_NOT_win() {
    return;
}
void warrior::attack(warrior* enemy) {
    if (enemy == nullptr)
        return;
    enemy->elements -= force;
    use_sword(enemy);
    cout << event_system::get_format_clock() << " " << *this << " attacked "
         << *enemy << " in city " << location << " with " << elements
         << " elements and force " << force << endl;
}
void warrior::fight_back(warrior* enemy) {
    if (enemy == nullptr)
        return;
    enemy->elements -= (force / 2);
    use_sword(enemy);
    cout << event_system::get_format_clock() << " " << *this
         << " fought back against " << *enemy << " in city " << location
         << endl;
}
void warrior::use_sword(warrior* enemy) {
    if (enemy == nullptr)
        return;
    int force = get_sword_force();
    enemy->elements -= force;
    if (weapons["sword"])
        (dynamic_cast<sword*>(weapons["sword"])->blunt());
}
int warrior::get_sword_force() {
    if (weapons["sword"] == nullptr)
        return 0;
    sword* sw = dynamic_cast<sword*>(weapons["sword"]);
    return sw->force;
}
bool warrior::check_if_use_bomb(warrior* enemy) {
    if (weapons["bomb"] == nullptr)
        return false;
    city& c = cities[location];
    if (!c.first || !c.second)
        return false;
    if (!c.first->is_alive() || !c.second->is_alive())
        return false;
    warrior* early_hand = c.who_attack_first();
    if (early_hand == this) {
        //如果后手方是ninja,因为ninja不会反击，所以先手放不会死，不会使用bomb
        if (enemy->name == "ninja")
            return false;
        // 先手打不死对方，且会被反击致死
        if (((force + get_sword_force()) < enemy->elements) &&
            ((enemy->force / 2 + enemy->get_sword_force()) >= elements))
            return true;
    } else {
        // 后手被对方打死
        if ((enemy->force + enemy->get_sword_force()) >= elements)
            return true;
    }
    return false;
}

void dragon::yell() {
    if (is_alive() && morale > 0.8)
        cout << event_system::get_format_clock() << " " << *this
             << " yelled in city " << location << endl;
}
void dragon::action_win() {
    morale += 0.2;
}
void dragon::action_NOT_win() {
    morale -= 0.2;
}

bool lion::escape() {
    // 狮子的逃跑函数，返回是否逃跑
    if (loyalty > 0)
        return false;
    else {
        cout << event_system::get_format_clock() << " " << *this
             << " ran away\n";
        return true;
    }
}
void lion::action_NOT_win() {
    loyalty -= loyalty_decrease;
}

void ninja::fight_back(warrior* w) {
    return;
}

void iceman::march() {
    int direction = part == "red" ? 1 : -1;  //红军为1向右走，-1向左走
    location += direction;
    steps++;
    element_decrease();
    cout << event_system::get_format_clock() << " " << *this
         << " marched to city " << location << " with " << elements
         << " elements and force " << force << endl;
}
void iceman::element_decrease() {
    if (steps % 2)
        return;
    if (elements > 9) {
        elements -= 9;
    } else {
        elements = 1;
    }
    force += 20;
}

void wolf::action_win() {
    warrior* enemy =
        (part == "red") ? cities[location].second : cities[location].first;
    for (string& weapon_name : weapon_kinds) {
        if (weapons[weapon_name] == nullptr) {
            weapons[weapon_name] = enemy->weapons[weapon_name];
            enemy->weapons[weapon_name] = nullptr;
        }
    }
}

void city::generate_elements() {
    elements += 10;
}

void city::raise_flag(flag flag_to_raise) {
    flag_status = flag_to_raise;
    string part = flag_to_raise == BLUE_FLAG ? "blue" : "red";
    cout << event_system::get_format_clock() << " " << part
         << " flag raised in city " << location << endl;
}

// 返回先手武士，有可能返回空指针！
warrior* city::who_attack_first() {
    if (flag_status == RED_FLAG)
        return first;
    if (flag_status == BLUE_FLAG)
        return second;
    if (flag_status == NO_FLAG && (location % 2))
        return first;
    return second;
}

fight_result city::check_fight_result() {
    if (first == nullptr || second == nullptr)
        return NO_WAR;
    else {
        if (first->is_alive() && second->is_alive())
            return BOTH_ALIVE;
        if (!first->is_alive() && second->is_alive())
            return BLUE_WIN;
        if (first->is_alive() && !second->is_alive())
            return RED_WIN;
        return BOTH_DEAD;
    }
}

int Base::blue_build_ind = 0;
int Base::red_build_ind = 0;

void Base::increase_build_inds(string part, int bound) {
    // bound 战士数目
    if ("red" == part) {
        Base::red_build_ind = (Base::red_build_ind + 1) % bound;
    } else {
        Base::blue_build_ind = (Base::blue_build_ind + 1) % bound;
    }
}
void Base::make_warriors() {
    warrior* p = further_process(make_warrior());
    if (p != nullptr) {
        event_system::warrior_born(*p);
        if (p->part == "red") {
            cities[0].first = p;
        } else {
            cities[cities.size() - 1].second = p;
        }
    }
}

warrior* Base::make_warrior() {
    warrior* ret = nullptr;
    int warrior_id = part_warrior_cnt[part] + 1;
    int ind = -1, location = -1;
    ind = part == "red" ? red_build_ind : blue_build_ind;
    location = part == "red" ? 0 : city_count + 1;
    int warrior_element_ready = warrior_element[warrior_build_order[part][ind]];
    if (warrior_element_ready <= cur_life_value) {
        part_warrior_cnt[part]++;
        cur_life_value -= warrior_element_ready;
        increase_build_inds(part, 5);
        warrior* p = new warrior(warrior_id, warrior_element_ready, part,
                                 warrior_build_order[part][ind], location,
                                 warrior_force[warrior_build_order[part][ind]]);
        ret = p;
    }
    return ret;
}

warrior* Base::further_process(warrior* p) {
    if (p == nullptr) {
        return nullptr;
    }
    string name = p->name;
    warrior* ret = nullptr;
    if (name == "dragon") {
        string weapon_name = weapon_kinds[p->id % 3];
        p->weapons[weapon_name] = make_weapon(p, weapon_name);
        double moral =
            (cur_life_value * 1.0) / (warrior_element["dragon"] * 1.0);
        ret = new dragon(*p, moral);
    } else if (name == "ninja") {
        string weapon_name1 = weapon_kinds[p->id % 3],
               weapon_name2 = weapon_kinds[(p->id + 1) % 3];
        p->weapons[weapon_name1] = make_weapon(p, weapon_name1);
        p->weapons[weapon_name2] = make_weapon(p, weapon_name2);
        ret = new ninja(*p);
    } else if (name == "iceman") {
        string weapon_name = weapon_kinds[p->id % 3];
        p->weapons[weapon_name] = make_weapon(p, weapon_name);
        ret = new iceman(*p);
    } else if (name == "lion") {
        ret = new lion(*p, cur_life_value);
    } else if (name == "wolf") {
        ret = new wolf(*p);
    }
    delete p;
    return ret;
}

weapon* Base::make_weapon(warrior* w, string weapon_name) {
    if (weapon_name == "sword") {
        int force = (w->force * 2) / 10;
        if (force)
            return new sword(weapon_name, force);
    } else if (weapon_name == "bomb") {
        return new bomb(weapon_name);
    } else if (weapon_name == "arrow") {
        return new arrow(weapon_name, arrow_force);
    }
    return nullptr;
}

void Base::award_warrior(warrior* w) {
    if (cur_life_value >= 8) {
        cur_life_value -= 8;
        w->elements += 8;
    }
}

int event_system::clock = 0;
bool event_system::conqured_blue = false;
bool event_system::conqured_red = false;

void event_system::happen00_warrior_born(Base& red, Base& blue) {
    red.make_warriors();
    blue.make_warriors();
}
void event_system::warrior_born(warrior& w) {
    cout << get_format_clock() << " " << w << " born\n";
    if (w.name == "dragon")
        cout << "Its morale is " << fixed << setprecision(2)
             << (dynamic_cast<dragon*>(&w))->morale << endl;
    else if (w.name == "lion")
        cout << "Its loyalty is " << (dynamic_cast<lion*>(&w))->loyalty << endl;
}
void event_system::time_go() {
    event_system::clock += 1;
}
string event_system::get_format_clock() {
    int hour = clock / 60;
    int minute = clock % 60;
    ostringstream ost;
    ost << setw(3) << setfill('0') << hour << ":";
    ost << setw(2) << setfill('0') << minute;
    return move(ost.str());
}
void event_system::happen05_lion_escape() {
    for (int i = 0; i < cities.size(); i++) {
        warrior *first = cities[i].first, *second = cities[i].second;
        if (first != nullptr && first->name == "lion") {
            lion* l = dynamic_cast<lion*>(first);
            if (l->escape()) {
                delete (cities[i].first);
                cities[i].first = nullptr;
            }
        }

        if (second != nullptr && second->name == "lion") {
            lion* l = dynamic_cast<lion*>(second);
            if (l->escape()) {
                delete (cities[i].second);
                cities[i].second = nullptr;
            }
        }
    }
}
void event_system::happen10_warrior_march() {
    warrior* b = cities[1].second;
    if (b != nullptr) {
        b->reach_headquarter();
        b->location = 0;
        cities[1].second = nullptr;
    }

    warrior* next_move_red = cities[0].first;
    cities[0].first = nullptr;
    for (int i = 1; i < cities.size() - 1; i++) {
        warrior *r = next_move_red, *b = cities[i + 1].second;
        next_move_red = cities[i].first;
        if (r != nullptr) {
            r->march();
        }
        cities[i].first = r;

        if (b != nullptr) {
            b->march();
        }
        cities[i].second = b;
    }
    cities[cities.size() - 1].second = nullptr;
    if (next_move_red != nullptr) {
        next_move_red->reach_headquarter();
        next_move_red->location = cities.size() - 1;
    }
}
void event_system::happen20_generate_elements() {
    for (int i = 1; i < cities.size() - 1; i++) {
        cities[i].generate_elements();
    }
}
void event_system::happen30_take_city_elements() {
    for (int i = 1; i < cities.size() - 1; i++) {
        if (cities[i].first == nullptr && cities[i].second != nullptr) {
            part2base["blue"]->cur_life_value += cities[i].elements;
            cout << get_format_clock() << " " << *cities[i].second << " earned "
                 << cities[i].elements << " elements for his headquarter\n";
            cities[i].elements = 0;
        }

        if (cities[i].first != nullptr && cities[i].second == nullptr) {
            part2base["red"]->cur_life_value += cities[i].elements;
            cout << get_format_clock() << " " << *cities[i].first << " earned "
                 << cities[i].elements << " elements for his headquarter\n";
            cities[i].elements = 0;
        }
    }
}
void event_system::happen35_shot_arrow() {
    //放箭射敌人,但是不清理战场。40分战斗结束后再清理战场
    for (int i = 1; i < cities.size() - 1; i++) {
        warrior* red = cities[i].first;
        warrior* blue = cities[i].second;
        warrior* next_blue = cities[i + 1].second;
        warrior* previous_red = cities[i - 1].first;
        if (red != nullptr && red->weapons["arrow"] != nullptr &&
            next_blue != nullptr) {
            red->weapons["arrow"]->attack(red, next_blue);
            cout << get_format_clock() << " " << *red << " shot";
            if (next_blue->is_alive())
                cout << "\n";
            else
                cout << " and killed " << *next_blue << endl;
        }

        if (blue != nullptr && blue->weapons["arrow"] != nullptr &&
            previous_red != nullptr) {
            blue->weapons["arrow"]->attack(blue, previous_red);
            cout << get_format_clock() << " " << *blue << " shot";
            if (previous_red->is_alive())
                cout << "\n";
            else
                cout << " and killed " << *previous_red << endl;
        }
    }
    // 如果一个城市的两个战士都被射死，视为没有发生战斗
    for (int i = 1; i < cities.size() - 1; i++) {
        if (!cities[i].first || !cities[i].second)
            continue;
        if (cities[i].first->is_alive() || cities[i].second->is_alive())
            continue;
        delete cities[i].first;
        cities[i].first = nullptr;
        delete cities[i].second;
        cities[i].second = nullptr;
    }
}
void event_system::happen38_use_bomb() {
    for (int i = 1; i < cities.size() - 1; i++) {
        warrior* red = cities[i].first;
        warrior* blue = cities[i].second;
        if (red != nullptr && red->check_if_use_bomb(blue)) {
            red->weapons["bomb"]->attack(red, blue);
            cout << get_format_clock() << " " << *red
                 << " used a bomb and killed " << *blue << endl;
            delete red;
            delete blue;
            cities[i].first = nullptr;
            cities[i].second = nullptr;
        }
        if (blue != nullptr && blue->check_if_use_bomb(red)) {
            blue->weapons["bomb"]->attack(blue, red);
            cout << get_format_clock() << " " << *blue
                 << " used a bomb and killed " << *red << endl;
            delete red;
            delete blue;
            cities[i].first = nullptr;
            cities[i].second = nullptr;
        }
    }
}
void event_system::happen40_war() {
    // 攻击和反攻
    int blue_earned = 0, red_earned = 0;
    for (int i = 1; i < cities.size() - 1; i++) {
        if (cities[i].first == nullptr || cities[i].second == nullptr)
            continue;
        warrior *red = cities[i].first, *blue = cities[i].second;
        warrior* early_hand = cities[i].who_attack_first();
        warrior* late_hand = early_hand == red ? blue : red;
        red->clear_weapons();
        blue->clear_weapons();
        // 发生了肉搏战斗
        if (red->is_alive() && blue->is_alive()) {
            int blue_lion_elements = blue->name == "lion" ? blue->elements : 0;
            int red_lion_elements = red->name == "lion" ? red->elements : 0;

            early_hand->attack(late_hand);  //攻击
            if (late_hand->is_alive()) {
                late_hand->fight_back(early_hand);  //反攻
            }
            // 战死. 肉搏战斗中只会至少有一方存活
            if (!red->is_alive()) {
                cout << get_format_clock() << " " << *red
                     << " was killed in city " << i << endl;

                blue->elements += red_lion_elements;
            }
            if (!blue->is_alive()) {
                cout << get_format_clock() << " " << *blue
                     << " was killed in city " << i << endl;

                red->elements += blue_lion_elements;
            }
        }
        red->clear_weapons();
        blue->clear_weapons();
        // 增减士气、忠诚度，抢夺武器
        take_actions_after_war(cities[i]);

        // 龙欢呼
        dragon* d = dynamic_cast<dragon*>(early_hand);
        if (d != nullptr)
            d->yell();

        // 收取城市生命元
        vector<int>&& v = get_city_elements_after_war(cities[i]);
        red_earned += v[0];
        blue_earned += v[1];

        // 旗帜升起
        raise_flag_after_war(cities[i]);
        change_prev_fight_result(cities[i]);
    }
    //奖励红武士
    for (int i = cities.size() - 2; i > 0; i--) {
        if (cities[i].check_fight_result() == RED_WIN)
            part2base["red"]->award_warrior(cities[i].first);
    }
    //奖励蓝武士
    for (int i = 1; i < cities.size() - 1; i++) {
        if (cities[i].check_fight_result() == BLUE_WIN)
            part2base["blue"]->award_warrior(cities[i].second);
    }
    part2base["red"]->cur_life_value += red_earned;
    part2base["blue"]->cur_life_value += blue_earned;
    // 清理战场
    for (city& c : cities) {
        if (c.first && !c.first->is_alive()) {
            delete c.first;
            c.first = nullptr;
        }
        if (c.second && !c.second->is_alive()) {
            delete c.second;
            c.second = nullptr;
        }
    }
}
void event_system::take_actions_after_war(city& c) {
    warrior *red = c.first, *blue = c.second;
    if (c.check_fight_result() == RED_WIN) {
        red->action_win();
    } else if (c.check_fight_result() == BLUE_WIN) {
        blue->action_win();
    } else if (c.check_fight_result() == BOTH_ALIVE) {
        blue->action_NOT_win();
        red->action_NOT_win();
    }
}
vector<int> event_system::get_city_elements_after_war(city& c) {
    warrior *red = c.first, *blue = c.second;
    int red_earned = 0, blue_earned = 0;
    if (c.check_fight_result() == RED_WIN) {
        red_earned += c.elements;

        cout << get_format_clock() << " " << *red << " earned " << c.elements
             << " elements for his headquarter\n";
        c.elements = 0;
    } else if (c.check_fight_result() == BLUE_WIN) {
        blue_earned += c.elements;
        cout << get_format_clock() << " " << *blue << " earned " << c.elements
             << " elements for his headquarter\n";
        c.elements = 0;
    }
    return {red_earned, blue_earned};
}
void event_system::raise_flag_after_war(city& c) {
    fight_result result = c.check_fight_result();
    if (result == c.previous_fight_result) {
        if (result == BLUE_WIN && c.flag_status != BLUE_FLAG) {
            c.raise_flag(BLUE_FLAG);
        } else if (result == RED_WIN && c.flag_status != RED_FLAG) {
            c.raise_flag(RED_FLAG);
        }
    }
}
void event_system::change_prev_fight_result(city& c) {
    fight_result result = c.check_fight_result();
    c.previous_fight_result = result;
}
void event_system::happen50_report_base(Base& red, Base& blue) {
    report_base(red);
    report_base(blue);
}
void event_system::report_base(Base& base) {
    cout << get_format_clock() << " " << base.cur_life_value << " elements in "
         << base.part << " headquarter\n";
}
void event_system::happen55_report_warriors() {
    for (int i = 1; i < cities.size() - 1; i++) {
        warrior* r = cities[i].first;
        report_warrior(r);
    }
    report_warrior(part2base["blue"]->first_arrived_enemy);
    report_warrior(part2base["red"]->first_arrived_enemy);
    for (int i = 1; i < cities.size() - 1; i++) {
        warrior* b = cities[i].second;
        report_warrior(b);
    }
}
void event_system::report_warrior(warrior* w) {
    if (w == nullptr)
        return;
    w->clear_weapons();
    cout << get_format_clock() << " " << *w << " has ";
    if (w->weapons["arrow"] || w->weapons["bomb"] || w->weapons["sword"]) {
        bool out1 = false, out2 = false;
        if (w->weapons["arrow"]) {
            cout << dynamic_cast<arrow*>(w->weapons["arrow"]);
            out1 = true;
        }
        if (w->weapons["bomb"]) {
            if (out1)
                cout << ",";
            cout << dynamic_cast<bomb*>(w->weapons["bomb"]);
            out2 = true;
        }
        if (w->weapons["sword"]) {
            if (out1 || out2)
                cout << ",";
            cout << dynamic_cast<sword*>(w->weapons["sword"]);
        }
        cout << endl;
    } else
        cout << "no weapon\n";
}

void __init__() {
    event_system::clock = 0;
    event_system::conqured_blue = false;
    event_system::conqured_red = false;
    Base::blue_build_ind = 0;
    Base::red_build_ind = 0;
    warrior_build_order["blue"] = blue_order;
    warrior_build_order["red"] = red_order;

    cities.clear();
    for (int i = 0; i < city_count + 2; i++) {
        cities.push_back(city(nullptr, nullptr, i));
    }
    part_warrior_cnt["red"] = 0;
    part_warrior_cnt["blue"] = 0;
    part2base["red"] = nullptr;
    part2base["blue"] = nullptr;
}
int main() {
    int tot;
    scanf("%d", &tot);
    for (int c = 1; c <= tot; c++) {
        printf("Case %d:\n", c);
        int tot_elements;
        cin >> tot_elements >> city_count >> arrow_force >> loyalty_decrease >>
            time_limit;
        cin >> warrior_element["dragon"] >> warrior_element["ninja"] >>
            warrior_element["iceman"] >> warrior_element["lion"] >>
            warrior_element["wolf"];
        cin >> warrior_force["dragon"] >> warrior_force["ninja"] >>
            warrior_force["iceman"] >> warrior_force["lion"] >>
            warrior_force["wolf"];
        __init__();
        Base red("red", tot_elements);
        Base blue("blue", tot_elements);
        part2base["red"] = &red;
        part2base["blue"] = &blue;
        while (event_system::clock <= time_limit &&
               (!event_system::conqured_blue && !event_system::conqured_red)) {
            if (event_system::clock % 60 == 0)
                event_system::happen00_warrior_born(red, blue);
            else if (event_system::clock % 60 == 5)
                event_system::happen05_lion_escape();
            else if (event_system::clock % 60 == 10)
                event_system::happen10_warrior_march();
            else if (event_system::clock % 60 == 20)
                event_system::happen20_generate_elements();
            else if (event_system::clock % 60 == 30)
                event_system::happen30_take_city_elements();
            else if (event_system::clock % 60 == 35)
                event_system::happen35_shot_arrow();
            else if (event_system::clock % 60 == 38)
                event_system::happen38_use_bomb();
            else if (event_system::clock % 60 == 40)
                event_system::happen40_war();
            else if (event_system::clock % 60 == 50)
                event_system::happen50_report_base(red, blue);
            else if (event_system::clock % 60 == 55) {
                event_system::happen55_report_warriors();
            }
            event_system::time_go();
        }
    }
}
