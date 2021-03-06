/*
 *  party.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "classes.h"
#include "oldstructs.h"

cParty& cParty::operator = (legacy::party_record_type& old){
	int i,j;
	age = old.age;
	gold = old.gold;
	food = old.food;
	for(i = 0; i < 310; i++)
		for(j = 0; j < 10; j++)
			stuff_done[i][j] = old.stuff_done[i][j];
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			item_taken[i][j] = old.item_taken[i][j];
	light_level = old.light_level;
	outdoor_corner.x = old.outdoor_corner.x;
	outdoor_corner.y = old.outdoor_corner.y;
	i_w_c.x = old.i_w_c.x;
	i_w_c.y = old.i_w_c.y;
	p_loc.x = old.p_loc.x;
	p_loc.y = old.p_loc.y;
	loc_in_sec.x = old.loc_in_sec.x;
	loc_in_sec.y = old.loc_in_sec.y;
	party_event_timers.reserve(30);
	for(i = 0; i < 30; i++){
		boats[i] = old.boats[i];
		horses[i] = old.horses[i];
		cTimer t;
		t.time = old.party_event_timers[i];
		t.global_or_town = old.global_or_town[i];
		t.node_to_call = old.node_to_call[i];
		party_event_timers.push_back(t);
//		party_event_timers[i].time = old.party_event_timers[i];
//		party_event_timers[i].global_or_town = old.global_or_town[i];
//		party_event_timers[i].node_to_call = old.node_to_call[i];
	}
	for(i = 0; i < 4; i++){
		creature_save[i] = old.creature_save[i];
		imprisoned_monst[i] = old.imprisoned_monst[i];
	}
	in_boat = old.in_boat;
	in_horse = old.in_horse;
	for(i = 0; i < 10; i++){
		out_c[i] = old.out_c[i];
		for(j = 0; j < 5; j++)
			magic_store_items[j][i] = old.magic_store_items[j][i];
	}
	for(i = 0; i < 256; i++)
		m_noted[i] = old.m_seen[i];
	journal.reserve(50);
	for(i = 0; i < 50; i++){
		cJournal j;
		j.str_num = old.journal_str[i];
		j.day = old.journal_day[i];
		journal.push_back(j);
//		journal[i].str_num = old.journal_str[i];
//		journal[i].day = old.journal_day[i];
		spec_items[i] = old.spec_items[i];
	}
	special_notes.reserve(140);
	for(i = 0; i < 140; i++){
		cEncNote n;
		n.str_num = old.special_notes_str[i][0];
		n.where = old.special_notes_str[i][1];
		special_notes.push_back(n);
//		special_notes[i].str_num = old.special_notes_str[i][0];
//		special_notes[i].where = old.special_notes_str[i][1];
	}
	talk_save.reserve(120);
	for(i = 0; i < 120; i++){
		cConvers t;
		t = old.talk_save[i];
		talk_save.push_back(t);
//		talk_save[i] = old.talk_save[i];
		help_received[i] = old.help_received[i];
	}
	direction = old.direction;
	at_which_save_slot = old.at_which_save_slot;
	for(i = 0; i < 20 ; i++)
		alchemy[i] = old.alchemy[i];
	for(i = 0; i < 200; i++){
		can_find_town[i] = old.can_find_town[i];
		m_killed[i] = old.m_killed[i];
	}
	for(i = 0; i < 100; i++)
		key_times[i] = old.key_times[i];
	total_m_killed = old.total_m_killed;
	total_dam_done = old.total_dam_done;
	total_xp_gained = old.total_xp_gained;
	total_dam_taken = old.total_dam_taken;
	scen_name = old.scen_name;
	return *this;
}

void cParty::append(legacy::stored_items_list_type& old,short which_list){
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i] = old.items[i];
}

void cParty::append(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
}

cParty::cConvers& cParty::cConvers::operator = (legacy::talk_save_type old){
	personality = old.personality;
	town_num = old.town_num;
	str_num1 = old.str1;
	str_num2 = old.str2;
	return *this;
}

void cParty::add_pc(legacy::pc_record_type old){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == 0){
			adven[i] = old;
			break;
		}
}

void cParty::void_pcs(){
	for(int i = 0; i < 6; i++)
		adven[i].main_status = MAIN_STATUS_ABSENT;
}

void cParty::add_pc(cPlayer new_pc){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == MAIN_STATUS_ABSENT){
			adven[i] = new_pc;
			break;
		}
}

bool cParty::has_talk_save(short who, short str1, short str2){
	for (unsigned int j = 0; j < talk_save.size(); j++)
		if ((talk_save[j].personality == who) && (talk_save[j].str_num1 == str1) &&	(talk_save[j].str_num2 == str2))
			return true;
	return false;
}

bool cParty::save_talk(short who, unsigned char where, short str1, short str2){
	if(talk_save.size() == talk_save.max_size()) return false; // This is extremely unlikely
	cConvers talk;
	talk.personality = who;
	talk.town_num = where;
	talk.str_num1 = str1;
	talk.str_num2 = str2;
	// TODO: locate the strings and store them in the record.
	talk_save.push_back(talk);
	return true;
}

bool cParty::add_to_journal(short event, short day){
	if(journal.size() == journal.max_size()) return false; // Practically impossible
	cJournal entry;
	entry.str_num = event;
	entry.day = day;
	// TODO: locate the strings and store them in the record.
	journal.push_back(entry);
	return true;
}

bool cParty::record(short what, short where){
	if(special_notes.size() == special_notes.max_size()) return false; // Never happen
	cEncNote note;
	note.str_num = what;
	note.where = where;
	// TODO: locate the strings and store them in the record.
	special_notes.push_back(note);
	return true;
}

bool cParty::start_timer(short time, short node, short type){
	if(party_event_timers.size() == party_event_timers.max_size()) return false; // Shouldn't be reached
	cTimer t;
	t.time = time;
	t.global_or_town = type;
	t.node_to_call = node;
	party_event_timers.push_back(t);
	return(true);
}

void cParty::writeTo(std::ostream& file){
	file << "AGE " << age << std::endl;
	file << "GOLD " << gold << std::endl;
	file << "FOOD " << food << std::endl;
	for(int i = 0; i < 310; i++)
		for(int j = 0; j < 50; j++)
			if(stuff_done[i][j] > 0)
				file << "SDF " << i << ' ' << j << ' ' << stuff_done[i][j] << std::endl;
	for(int i = 0; i < 200; i++)
		if(item_taken[i][0] > 0 || item_taken[i][1] > 0 || item_taken[i][2] > 0 || item_taken[i][3] > 0 ||
		   item_taken[i][4] > 0 || item_taken[i][5] > 0 || item_taken[i][6] > 0 || item_taken[i][7] > 0)
			file << "ITEMTAKEN " << i << ' ' << item_taken[i][0] << ' ' << item_taken[i][1] << ' '
				 << item_taken[i][2] << ' ' << item_taken[i][3] << ' ' << item_taken[i][4] << ' ' << item_taken[i][5]
				 << ' ' << item_taken[i][6] << ' ' << item_taken[i][7] << std::endl;
	file << "LIGHT " << light_level << std::endl;
	file << "OUTCORNER " << outdoor_corner.x << ' ' << outdoor_corner.y << std::endl;
	file << "INWHICHCORNER " << i_w_c.x << ' ' << i_w_c.y << std::endl;
	file << "SECTOR " << p_loc.x << ' ' << p_loc.y << std::endl;
	file << "LOCINSECTOR " << loc_in_sec.x << ' ' << loc_in_sec.y << std::endl;
	// TODO: Delegate this to the cVehicle class
	for(int i = 0; i < 30; i++){
		if(!boats[i].exists) continue;
		file << "BOAT " << i << ' ';
		file << boats[i].loc.x << ' ' << boats[i].loc.y << ' ';
		file << boats[i].loc_in_sec.x << ' ' << boats[i].loc_in_sec.y << ' ';
		file << boats[i].sector.x << ' ' << boats[i].sector.y << ' ';
		file << boats[i].which_town << ' ' << (short)boats[i].property << std::endl;
	}
	for(int i = 0; i < 30; i++){
		if(!horses[i].exists) continue;
		file << "HORSE " << i << ' ';
		file << horses[i].loc.x << ' ' << horses[i].loc.y << ' ';
		file << horses[i].loc_in_sec.x << ' ' << horses[i].loc_in_sec.y << ' ';
		file << horses[i].sector.x << ' ' << horses[i].sector.y << ' ';
		file << horses[i].which_town << ' ' << (short)horses[i].property << std::endl;
	}
	file << "IN " << in_boat << ' ' << in_horse << std::endl;
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 50; j++)
			if(magic_store_items[i][j].variety > ITEM_TYPE_NO_ITEM){
				std::ostringstream sout;
				sout << "MAGICSTORE " << i << ' ' << j << ' ';
				magic_store_items[i][j].writeTo(file, sout.str());
			}
	for(int i = 0; i < 256; i++)
		if(m_noted[i])
			file << "ROSTER " << i << std::endl;
	for(int i = 0; i < 256; i++)
		if(m_seen[i])
			file << "SEEN " << i << std::endl;
	for(int i = 0; i < 10; i++)
		if(out_c[i].exists){
			file << "ENCOUNTER " << i << " DIRECTION " << out_c[i].direction << std::endl;
			file << "ENCOUNTER " << i << " SECTOR " << out_c[i].which_sector.x << ' ' << out_c[i].which_sector.y << std::endl;
			file << "ENCOUNTER " << i << " LOCINSECTOR " << out_c[i].m_loc.x << ' ' << out_c[i].m_loc.y << std::endl;
			file << "ENCOUNTER " << i << " HOME " << out_c[i].home_sector.x << ' ' << out_c[i].home_sector.y << std::endl;
			std::ostringstream sout;
			sout << "ENCOUNTER " << i << ' ';
			out_c[i].what_monst.writeTo(file,sout.str());
		}
	for(int i = 0; i < 4; i++)
		if(imprisoned_monst[i] > 0)
			file << "SOULCRYSTAL " << i << ' ' << imprisoned_monst[i] << std::endl;
	file << "DIRECTION " << direction << std::endl;
	file << "WHICHSLOT " << at_which_save_slot << std::endl;
	for(int i = 0; i < 20; i++)
		if(alchemy[i])
			file << "ALCHEMY " << i << std::endl;
	for(int i = 0; i < 200; i++)
		if(can_find_town[i])
			file << "TOWN " << i << std::endl;
	for(int i = 0; i < 100; i++)
		if(key_times[i])
			file << "EVENT " << i << ' ' << key_times[i] << std::endl;
	for(int i = 0; i < 50; i++)
		if(spec_items[i])
			file << "ITEM " << i << std::endl;
	for(int i = 0; i < 120; i++)
		if(help_received[i])
			file << "HELP " << i << std::endl;
	for(int i = 0; i < 200; i++)
		if(m_killed[i] > 0)
			file << "TOWNSLAUGHTER " << i << ' ' << m_killed[i] << std::endl;
	file << "KILLS " << total_m_killed << std::endl;
	file << "DAMAGE " << total_dam_done << std::endl;
	file << "WOUNDS " << total_dam_taken << std::endl;
	file << "EXPERIENCE " << total_xp_gained << std::endl;
	file << "SCENARIO " << scen_name << std::endl;
	file << "WON " << scen_won << std::endl;
	file << "PLAYED " << scen_played << std::endl;
	for(campIter iter = campaign_flags.begin(); iter != campaign_flags.end(); iter++){
		for(unsigned int i = 0; i < iter->second.size(); i++)
			if(iter->second[i] > 0)
				file << "CAMPAIGN \"" << iter->first << "\" " << i << ' ' << iter->second[i] << std::endl;
	}
	for(unsigned int i = 0; i < 250; i++)
		if(graphicUsed[i])
			file << "GRAPHIC " << i << std::endl;
	for(unsigned int i = 0; i < party_event_timers.size(); i++)
		file << "TIMER " << i << ' ' << party_event_timers[i].time << ' ' << party_event_timers[i].global_or_town
			 << ' ' << party_event_timers[i].node_to_call << std::endl;
	file << '\f';
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 64; j++){
			file << setup[i][j][0];
			for(int k = 1; k < 64; k++)
				file << '\t' << setup[i][j][k];
			file << std::endl;
		}
		file << '\f';
	}
	// TODO: The three journal note variables
	// TODO: creature_save
	// TODO: stored_items
	// TODO: summons
}

void cParty::readFrom(std::istream& file){
	// TODO: Error-check input
	std::istringstream bin, sin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		sin.str(cur);
		sin >> cur;
		if(cur == "AGE")
			sin >> age;
		else if(cur == "GOLD")
			sin >> gold;
		else if(cur == "FOOD")
			sin >> food;
		else if(cur == "SDF"){
			int i,j;
			sin >> i >> j;
			sin >> stuff_done[i][j];
		}else if(cur == "ITEMTAKEN"){
			int i;
			sin >> i;
			sin >> item_taken[i][0] >> item_taken[i][1] >> item_taken[i][2] >> item_taken[i][3]
				>> item_taken[i][4] >> item_taken[i][5] >> item_taken[i][6] >> item_taken[i][7];
		}else if(cur == "LIGHT")
			sin >> light_level;
		else if(cur == "OUTCORNER")
			sin >> outdoor_corner.x >> outdoor_corner.y;
		else if(cur == "INWHICHCORNER")
			sin >> i_w_c.x >> i_w_c.y;
		else if(cur == "SECTOR")
			sin >> p_loc.x >> p_loc.y;
		else if(cur == "LOCINSECTOR")
			sin >> loc_in_sec.x >> loc_in_sec.y;
		else if(cur == "BOAT"){
			int i;
			sin >> i;
			sin >> boats[i].loc.x >> boats[i].loc.y >> boats[i].loc_in_sec.x >> boats[i].loc_in_sec.y
				>> boats[i].sector.x >> boats[i].sector.y >> boats[i].which_town >> boats[i].property;
			boats[i].exists = true;
		}else if(cur == "HORSE"){
			int i;
			sin >> i;
			sin >> horses[i].loc.x >> horses[i].loc.y >> horses[i].loc_in_sec.x >> horses[i].loc_in_sec.y
				>> horses[i].sector.x >> horses[i].sector.y >> horses[i].which_town >> horses[i].property;
			horses[i].exists = true;
		}else if(cur == "IN")
			sin >> in_boat >> in_horse;
		else if(cur == "MAGICSTORE"){
			int i,j;
			sin >> i >> j >> cur;
			magic_store_items[i][j].readAttrFrom(cur,sin);
		}else if(cur == "ROSTER"){
			int i;
			sin >> i;
			m_noted[i] = true;
		}else if(cur == "SEEN"){
			int i;
			sin >> i;
			m_seen[i] = true;
		}else if(cur == "ENCOUNTER"){
			int i;
			sin >> i >> cur;
			if(cur == "DIRECTION")
				sin >> out_c[i].direction;
			else if(cur == "SECTOR")
				sin >> out_c[i].which_sector.x >> out_c[i].which_sector.y;
			else if(cur == "LOCINSECTOR")
				sin >> out_c[i].m_loc.x >> out_c[i].m_loc.y;
			else if(cur == "HOME")
				sin >> out_c[i].home_sector.x >> out_c[i].home_sector.y;
			else out_c[i].what_monst.readAttrFrom(cur,sin);
		}else if(cur == "SOULCRYSTAL"){
			int i;
			sin >> i;
			sin >> imprisoned_monst[i];
		}else if(cur == "DIRECTION")
			sin >> direction;
		else if(cur == "WHICHSLOT")
			sin >> at_which_save_slot;
		else if(cur == "ALCHEMY"){
			int i;
			sin >> i;
			alchemy[i] = true;
		}else if(cur == "TOWN"){
			int i;
			sin >> i;
			can_find_town[i] = true;
		}else if(cur == "EVENT"){
			int i;
			sin >> i;
			sin >> key_times[i];
		}else if(cur == "ITEM"){
			int i;
			sin >> i;
			spec_items[i] = true;
		}else if(cur == "HELP"){
			int i;
			sin >> i;
			help_received[i] = true;
		}else if(cur == "TOWNSLAUGHTER"){
			int i;
			sin >> i;
			sin >> m_killed[i];
		}else if(cur == "KILLS")
			sin >> total_m_killed;
		else if(cur == "DAMAGE")
			sin >> total_dam_done;
		else if(cur == "WOUNDS")
			sin >> total_dam_taken;
		else if(cur == "EXPERIENCE")
			sin >> total_xp_gained;
		else if(cur == "SCENARIO")
			sin >> scen_name;
		else if(cur == "WON")
			sin >> scen_won;
		else if(cur == "PLAYED")
			sin >> scen_played;
		else if(cur == "CAMPAIGN"){
			unsigned int i;
			int j;
			if(sin.peek() == '"'){
				sin.get();
				getline(sin, cur, '"');
			}else sin >> cur;
			sin >> i >> j;
			while(campaign_flags[cur].size() < i) campaign_flags[cur].push_back(0);
			campaign_flags[cur][i] = j;
		}else if(cur == "GRAPHIC"){
			int i;
			sin >> i;
			graphicUsed[i] = true;
		}else if(cur == "TIMER"){
			int i;
			sin >> i;
			sin >> party_event_timers[i].time >> party_event_timers[i].global_or_town >> party_event_timers[i].node_to_call;
		}
	}
	getline(file, cur, '\f');
	bin.str(cur);
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 64; j++)
			for(int k = 0; k < 64; k++)
				bin >> setup[i][j][k];
}

cPlayer& cParty::operator[](unsigned short n){
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6) return adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return adven[n];
}

void cParty::set_ptr(short p, unsigned short sdfx, unsigned short sdfy){ // This function is not used for setting the reserved pointers
	if(p >= -199 && p <= -100){ // must be a mutable pointer
		if(sdfx >= 300) throw std::range_error("SDF x-coordinate out of range (0..299)");
		if(sdfy >= 50) throw std::range_error("SDF y-coordinate out of range (0..49)");
		pointers[p] = std::make_pair(sdfx,sdfy);
	}
	else throw std::range_error("Pointer out of range (-199 to -100)");
}

void cParty::force_ptr(short p, unsigned short sdfx, unsigned short sdfy){
	pointers[p] = std::make_pair(sdfx,sdfy);
}

unsigned char cParty::get_ptr(short p){
	ptrIter iter = pointers.find(p);
	if(iter == pointers.end()) return 0;
	return stuff_done[iter->second.first][iter->second.second];
}

bool cParty::is_split(){
	bool ret = false;
	for(int i = 0; i < 6; i++)
		if(!stuff_done[304][i])
			ret = true;
	return ret;
}

bool cParty::pc_present(short i){
	if(i >= 6 || i < 0) return false;
	return stuff_done[304][i];
}

location cParty::left_at(){
	return loc(stuff_done[304][6],stuff_done[304][7]);
}

size_t cParty::left_in(){
	return stuff_done[304][8];
}
extern cUniverse univ;
std::string cParty::start_split(short a,short b,snd_num_t noise,short who) {
	short i;
	
	if(who >= 6 || who < 0) return "";
	if(is_split())
		return "Party already split!";
	stuff_done[304][who] = 0;
	stuff_done[304][6] = univ.town.p_loc.x;
	stuff_done[304][7] = univ.town.p_loc.y;
	stuff_done[304][8] = univ.town.num;
	univ.town.p_loc.x = a;
	univ.town.p_loc.y = b;
	for (i = 0; i < 6; i++)
		if (!stuff_done[304][who])
			adven[i].main_status += MAIN_STATUS_SPLIT;
	if (noise > 0)
		play_sound(10);
	return "";
}

std::string cParty::end_split(snd_num_t noise) {
	short i;
	
	if (!is_split())
		return "Party already together!";
	univ.town.p_loc = left_at();
	univ.town.num = left_in();
	for (i = 0; i < 6; i++){
		if (univ.party[i].main_status >= MAIN_STATUS_SPLIT)
			univ.party[i].main_status -= MAIN_STATUS_SPLIT;
		stuff_done[304][i] = true;
	}
	if (noise > 0)
		play_sound(10);	
	return "You are reunited.";
}

short cParty::pc_present(){
	short ret = 7;
	for(int i = 0; i < 6; i++){
		if(stuff_done[304][i] && ret == 7)
			ret = i;
		else if(stuff_done[304][i] && ret < 6)
			ret = 6;
	}
	if(ret == 7) ret = 6;
	return ret;
}
