#include "aJson.h"

void runProgram(aJsonObject* arrayObject) {
	int currentCommand = 0;
	int maxCommand = aJson.getArraySize(arrayObject);
	bool maze_completed = false;
	bool lastBooleanValue = false;

	while(currentCommand < maxCommand) {
		aJsonObject* commandArray = aJson.getArrayItem(arrayObject, currentCommand);
		int command = commandArray.getArrayItem(commandArray, 0);
		switch(command) {
			case 01: // label
				++currentCommand;
				break;
			case 02: // goto_if_true
				if(lastBooleanValue) {

				}
				break;
			case 03: // goto_if_false
				if(!lastBooleanValue) {

				}
				break;
			case 04: // goto

				break;
			case 04: // turn (01: left, 02: right)
				int argument = commandArray.getArrayItem(commandArray, 0)->valueint;
				if(argument == 1) {
					// turn left
				} else {
					//turn right
				}
				break;
			case 06: // move_forward
				//check to ensure no obstructions
				//if obstruction
					//exit
				//else
					//move forward
				break;
			case 07: // maze_completed
				maze_completed = true;
				break;
			case 08: // is_facing_wall

				break;
			case 09: // is_completed

				break;
			case 10: // is_facing_color (01: red, 02: green)

				break;
			case 11: // and

				break;
			case 12: // or

				break;
			case 13: // not

				break;
			case 14: // highlight block
				//make api web request
				//deserialize response
				//ensure running = true
				//if not
					//return
				break;
		}
	}

	return;
}

int main() {
	aJsonObject* root = aJson.parse("{\"running\": false, \"program\": [[14, 15], [1, 1], [13, [9]], [2, 2], [3, 3], [1, 2], [14, 18], [11, [12, [8], [10, 1]], [9]], [2, 4], [3, 5], [1, 4], [14, 25], [10, 2], [2, 6], [3, 7], [1, 6], [14, 28], [5, 1], [14, 30], [5, 2], [4, 8], [1, 7], [14, 32], [7], [14, 33], [6], [1, 8], [1, 5], [4, 1], [1, 3]]}");

	aJsonObject* running = aJson.getObjectItem(root, "running");
	aJsonObject* program = aJson.getObjectItem(root, "program");

	if(running->valuebool) {
		runProgram(program->);
	}

	//delay and try again

	return 0;
}