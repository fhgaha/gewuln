#pragma once

#include <stdio.h>

class Character;

class CharacterState
{
public:
	virtual ~CharacterState(){};
	virtual void 			enter			(Character& character) = 0;
	virtual CharacterState* process_input	(Character& character, bool *Keys, bool *KeysProcessed, const float dt) = 0;
	virtual void 			update			(Character& character, const float dt) = 0;
	virtual void 			exit			(Character& character) = 0;
};

class IdleState: public CharacterState
{
public:
	IdleState(){}
	virtual void 			enter			(Character& character) override;
	virtual CharacterState* process_input	(Character& character, bool *Keys, bool *KeysProcessed, const float dt) override;
	virtual void 			update			(Character& character, const float dt) override;
	virtual void 			exit			(Character& character) override;
};

class WalkState: public CharacterState
{
public:
	WalkState(){}
	virtual void 			enter			(Character& character) override;
	virtual CharacterState* process_input	(Character& character, bool *Keys, bool *KeysProcessed, const float dt) override;
	virtual void 			update			(Character& character, const float dt) override;
	virtual void 			exit			(Character& character) override;
};

class InteractState: public CharacterState
{
public:
	InteractState(){}
	virtual void 			enter			(Character& character) override;
	virtual CharacterState* process_input	(Character& character, bool *Keys, bool *KeysProcessed, const float dt) override;
	virtual void 			update			(Character& character, const float dt) override;
	virtual void 			exit			(Character& character) override;
};

#include <gewuln/character.h>
