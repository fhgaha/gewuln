#pragma once

#include <stdio.h>

class Character;

class CharacterState
{
public:
	virtual ~CharacterState(){};
	virtual CharacterState* process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt) = 0;
	virtual void update(Character& character, const float dt) = 0;
};


class IdleState: public CharacterState
{
public:
	IdleState(){}
	virtual CharacterState* process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt) override;
	virtual void update(Character& character, const float dt) override;
};

class WalkState: public CharacterState
{
public:
	WalkState(){}
	virtual CharacterState* process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt) override;
	virtual void update(Character& character, const float dt) override;
};
