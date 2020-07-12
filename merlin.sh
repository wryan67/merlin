#!/bin/ksh
set -a

AUDIODEV='default:CARD=Device'

MCP23017_ADDRESS="0x20"
MCP23017_INTA_PIN=7
MCP23017_INTB_PIN=0

KEYPAD_0="${MCP23017_ADDRESS}_A_6"
KEYPAD_1="${MCP23017_ADDRESS}_A_3"
KEYPAD_2="${MCP23017_ADDRESS}_A_0"
KEYPAD_3="${MCP23017_ADDRESS}_B_5"
KEYPAD_4="${MCP23017_ADDRESS}_A_4"
KEYPAD_5="${MCP23017_ADDRESS}_A_1"
KEYPAD_6="${MCP23017_ADDRESS}_B_4"
KEYPAD_7="${MCP23017_ADDRESS}_A_5"
KEYPAD_8="${MCP23017_ADDRESS}_A_2"
KEYPAD_9="${MCP23017_ADDRESS}_B_3"
KEYPAD_10="${MCP23017_ADDRESS}_B_1"

NEW_GAME="${MCP23017_ADDRESS}_B_7"
SAME_GAME="${MCP23017_ADDRESS}_B_6"
HIT_ME="${MCP23017_ADDRESS}_A_7"
COMP_TURN="${MCP23017_ADDRESS}_B_0"

ps -ef | awk '{if (/merlin.out/ && !/awk/) system(sprintf("sudo kill %d",$2))}'

if [ ! -s /home/wryan/projects/merlin/bin/ARM/Debug/merlin.out ];then
  echo game is not compiled
  exit 2
fi

echo starting merlin...
sudo -E $HOME/projects/merlin/bin/ARM/Debug/merlin.out
