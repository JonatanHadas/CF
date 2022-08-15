#ifndef _GAME_SETTINGS_H
#define _GAME_SETTINGS_H

#include "game_data.h"

#include <vector>
#include <set>

using namespace std;

class GameSettings{
public:
	GameSettings(
		ScoreSettings scores,
		bool using_teams,
		const vector<string>& team_names,
		const vector<int>& teams,
		const set<PowerUpDescriptor>& allowed_powerups,
		const vector<string>& names,
		const vector<int>& colors
	);

	ScoreSettings scores;

	bool using_teams;

	vector<string> team_names;
	vector<int> teams;
	
	set<PowerUpDescriptor> allowed_powerups;
	
	vector<string> names;
	vector<int> colors;
	
	vector<int> get_teams() const;
	
	void serialize(ostream& output) const;
	static GameSettings deserialize(istream& input);
};

#endif
