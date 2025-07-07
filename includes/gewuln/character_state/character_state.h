#pragma once

#include <gewuln/character.h>

class Character;

class CharacterState
{
public:
	virtual ~CharacterState(){};
	virtual CharacterState *process_input(Character& character) = 0;
	virtual void update(Character& character) = 0;
};

class IdleState: public CharacterState
{
public:
	IdleState(){}
	virtual CharacterState *process_input(Character& character) override;
	virtual void update(Character& character) override;
};

class WalkState: public CharacterState
{
public:
	WalkState(){}
	virtual CharacterState *process_input(Character& character) override;
	virtual void update(Character& character) override;
};

inline CharacterState *IdleState::process_input(Character& character)  
{
	if (true) {
		return new WalkState();
	}
	
	return nullptr;
}

inline void IdleState::update(Character& character)
{
	
}


inline CharacterState *WalkState::process_input(Character& character)
{
	if (true) {
		return new IdleState();
	}
	
	return nullptr;
}

inline void WalkState::update(Character& character)
{
	
}