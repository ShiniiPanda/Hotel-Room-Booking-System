#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define NUMBER_OF_ROOMS 10
#define MAX_B_RECORDS 40
#define SERVICE_TAX 0.1
#define TOURISM_TAX 10

typedef struct Rooms {
	char ID[4];
	char type[20];
	int price;
	char status[9];
} Room;

typedef struct Bookings {
	char passportIC[20];
	char status[9];
	char ID[4];
	char name[40];
	char email[64];
	char contact_num[16];
	int DOS;
	char BD[20];
	char ND[10];
} Booking;

typedef struct Date {
	int day;
	int	month;
	int year;
} Date;

char delimiter[2] = "||";

void main_menu();

void add_booking(struct Bookings *b) { // Append a record to Bookings.txt
	FILE* ptrB = fopen("Bookings.txt", "a");
	fprintf(ptrB, "%s||%s||%s||%s||%s||%s||%s||%d||%s\n", b->passportIC, b->status, b->ND, b->ID, b->name, b->email, b->contact_num, b->DOS, b->BD);
	fclose(ptrB);
}

void update_bookings(char lines[MAX_B_RECORDS][205], int num) { // Completely Replace Bookings.txt with new array of lines.
	FILE* ptrB = fopen("Bookings.txt", "w");
	if (ptrB == NULL) {
		printf("File cannot be created!");
		return;
	}
	for (int i = 0; i < num; i++) {
		fprintf(ptrB, "%s", lines[i]);
	}
	fclose(ptrB);
}

void update_record(struct Bookings* b, int line_num) {
	FILE* ptrB = fopen("Bookings.txt", "r");
	char lines[MAX_B_RECORDS][205];
	int num = 0;
	while (fgets(lines[num], 205, ptrB)) {
		num++;
	}
	snprintf(lines[line_num], 205, "%s||%s||%s||%s||%s||%s||%s||%d||%s\n", b->passportIC, b->status, b->ND, b->ID, b->name, b->email, b->contact_num, b->DOS, b->BD);
	fclose(ptrB);
	update_bookings(lines, num);
}

void update_rooms(char lines[NUMBER_OF_ROOMS][40]) { // Completely replace the Rooms.txt file with an array of lines.
	FILE* ptrR = fopen("Rooms.txt", "w");
	for (int i = 0; i < NUMBER_OF_ROOMS; i++) {
		fprintf(ptrR, "%s", lines[i]);
	}
	fclose(ptrR);
}

int fetch_details(char pass_ic[20], struct Bookings *b, struct Rooms *r, int *line_nums) {
	FILE* ptrR = fopen("Rooms.txt", "r"), * ptrB = fopen("Bookings.txt", "r");
	if (ptrR == NULL || ptrB == NULL) {
		printf("Make sure Rooms.txt and Bookings.txt are available.\n");
		return 0;
	}
	char b_record[205], r_record[40], * pline = 000000;
	struct Booking* initialPtr = b;
	int b_num = 0, r_num = 0, line_num = -1;
	while (fgets(b_record, 205, ptrB)) { // Fetching every line from Bookings.txt
		line_num++;
		pline = strtok(b_record, delimiter); // Fetching the Passport/IC Number into pline.
		if (strcmp(pline, pass_ic) != 0) continue; // If the passport/IC doesn't match, the loop iterates.
		#pragma region booking data
		strncpy(b->passportIC, pline, 20);
		pline = strtok(NULL, delimiter); // Otherwise we take the value of the status and store it into pline.
		strncpy(b->status, pline, 9);
		pline = strtok(NULL, delimiter);
		strncpy(b->ND, pline, 10);
		pline = strtok(NULL, delimiter);
		strncpy(b->ID, pline, 4);
		pline = strtok(NULL, delimiter);
		strncpy(b->name, pline, 40);
		pline = strtok(NULL, delimiter);
		strncpy(b->email, pline, 64);
		pline = strtok(NULL, delimiter);
		strncpy(b->contact_num, pline, 16);
		pline = strtok(NULL, delimiter);
		b->DOS = atoi(pline);
		pline = strtok(NULL, delimiter);
		strncpy(b->BD, pline, 20);
		(b->BD)[strcspn(b->BD, "\n")] = 0;
		*line_nums = line_num;
		#pragma endregion booking data
		b_num++;
		b++;
		line_nums++;
	}
	fclose(ptrB);
	if (b_num == 0) return b_num; // No record found for given passport / IC number then we exit function and return 0.
	b = initialPtr;
	while (r_num < b_num) {
		rewind(ptrR);
		while (fgets(r_record, 40, ptrR)) {
			pline = strtok(r_record, delimiter);
			if (strcmp(b->ID, pline) != 0) continue;
			//	room[r_num] = malloc(sizeof(struct Rooms));
			strncpy(r->ID, pline, 4);
			pline = strtok(NULL, delimiter);
			strncpy(r->type, pline, 18);
			pline = strtok(NULL, delimiter);
			r->price = atoi(pline);
			r_num++;
			r++;
			b++;
			break;
		}
	}
	fclose(ptrR);
	//Fetch Record;
	return b_num;
}

int check_expiredB(int *IDs) {
	// This function is used at the beginning of the program to check for expired bookings.
	FILE* ptrB = fopen("Bookings.txt", "r");
	if (ptrB == NULL) {
		return printf("Unable to access Bookings.txt\n");
	}
	char line[205], remaining[140], lines[MAX_B_RECORDS][205],  *pline = 000000, *pdate = 000000; // CHANGE LINES VALUE
	char pass_ic[20], ID[4]; // Varibles to store passport/IC and Room ID
	int num = 0, line_num = 0;
	Date date;
	Booking booking;
	time_t tnull = time(NULL);
	struct tm current_date = *localtime(&tnull);
	while (fgets(line, 205, ptrB)) {
		strcpy(lines[line_num], line);
		line_num++;
		pline = strtok(line, delimiter);
		strncpy(booking.passportIC, pline, 20);
		pline = strtok(NULL, delimiter); // Fetch the Booking Status.
		if (strncmp(pline, "ACTIVE", 6) != 0) continue; // If Booking status is not Active, ignore and move on.
		pline = strtok(NULL, delimiter); // Fetch the due date of the booking record.
		// Split the date into Day, Month and Year.
		pdate = strtok(pline, "/");
		date.month = atoi(pdate);
		pdate = strtok(NULL, "/");
		date.day = atoi(pdate);
		pdate = strtok(NULL, "/");
		date.year = atoi(pdate);
		// Check if date is still ahead of localtime.. if so then move on.
		if (((date.year >= current_date.tm_year - 100)
			&& (date.month > current_date.tm_mon + 1))
			|| ((date.year == current_date.tm_year - 100) &&
				(date.month == current_date.tm_mon + 1) &&
				(date.day >= current_date.tm_mday))) continue;
		// If expired record found do the following
		pline = pdate + 4; // Increment pline pointer past the date.
		strcpy(remaining, pline); // Copy the rest of line (which won't change) into line variable
		strncpy(booking.ID, strtok(pline, delimiter), 4); // Save the booking.ID to pass into the main function
		IDs[num] = atoi(booking.ID); // Assing the booking ID to the passed IDs int array.
		num++; // Increment number of expired lines found .. if this number is larger than 1 the main function will proceed to check IDs array.
		snprintf(lines[line_num - 1], 205, "%s||OUTDATED||%d/%d/%d||%s", booking.passportIC, date.month, date.day, date.year, remaining); // Edit the specific expired line.
	}
	fclose(ptrB);
	if (num > 0) {
		update_bookings(lines, line_num);
	}
	return num;
}

void check_occupation() {
	// This function is used at the beginning of the program to free up rooms linked to expired bookings.
	FILE* ptrR = fopen("Rooms.txt", "r");
	if (ptrR == NULL) {
		return printf("Unable to access Rooms.txt\n");
	}
	int IDs[NUMBER_OF_ROOMS], num = 0, ID, found;
	char line[40], lines[NUMBER_OF_ROOMS][40], * pline = 000000, line_args[4][20];
	int change_num = check_expiredB(&IDs), line_num = 0;
	if (change_num == 0) return;
	while (fgets(line, 40, ptrR)) {
		found = 0;
		strncpy(lines[line_num], line, 40);
		line_num++;
		if (num >= change_num) continue;
		pline = strtok(line, delimiter);
		ID = atoi(pline);
		for (int i = 0; i < change_num; i++) {
			if (ID == IDs[i]) {
				found = 1;
				break;
			}
		}
		if (found != 1) continue; // If Room ID is not found in IDs then loop continues to next record.
		for (int j = 0; j < 3; j++) {
			strcpy(line_args[j], pline);
			pline = strtok(NULL, delimiter);
		}
		snprintf(lines[line_num - 1], 40, "%s||%s||%s||OPEN\n", line_args[0], line_args[1], line_args[2]);
		num++;
	}
	fclose(ptrR);
	if (num > 0) {
		update_rooms(lines);
	}
}

int check_viable(char *pass_ic) { // This function checks if a booking was already made by a specific Passport/IC Number.
	FILE* ptrB = fopen("Bookings.txt", "r"); // Accessing Bookings.txt
	char line[40], * pline = 000000;
	while (fgets(line, 40, ptrB)) { // Fetching every line from Bookings.txt
		pline = strtok(line, delimiter); // Fetching the Passport/IC Number into pline.
		if (strcmp(pline, pass_ic) != 0) continue; // If the passport/IC doesn't match, the loop iterates.
		pline = strtok(NULL, delimiter); // Otherwise we take the value of the status and store it into pline.
		if (strcmp(pline, "ACTIVE") == 0) return 0; // If the status is "ACTIVE", we exit out the function and return 0.
	}
	fclose(ptrB); // Closing file..
	return 1; // If the loop didn't find any active bookings for the provided passport/ic, it returns 1 indicating valid passport/ic.
}

void view_rooms() {
	// Function to view all room records in Rooms.txt
	FILE* ptrR = fopen("Rooms.txt", "r");
	if (ptrR == NULL) {
		printf("Unable to access Rooms.txt file, make sure file exists.\n");
		return main_menu();
	}
	int count = 1;;
	char line[40], * pline = 000000, line_args[4][20];
	printf("===========================================\n");
	printf("                Hotel Rooms                \n");
	printf("===========================================\n\n");
	while (fgets(line, 40, ptrR)) {
		pline = strtok(line, delimiter);
		for (int i = 0; pline != NULL; i++) {
			strcpy(line_args[i], pline);
			pline = strtok(NULL, delimiter);
		}
		printf("(%2d) ID: %s || Type: %-15s || Price: %-4s Ringgit/Day || Status: %s", count, line_args[0], line_args[1], line_args[2], line_args[3]);
		count++;
	}
	printf("\n===========================================\n");
	fclose(ptrR);
	printf("Press any key to return to menu....\n");
	_getch();
	main_menu();
}

void get_name(struct Bookings *booking) {
	printf("Enter Full Name: ");
	fgets(booking->name, 40, stdin);
	if((booking->name)[strlen(booking->name) - 1] == '\n') (booking->name)[strlen(booking->name) - 1] = '\0';
}

void get_passport_ic(struct Bookings* booking) {
	printf("Enter Passport/IC Number: ");
	scanf("%19s", booking->passportIC);
}

void get_contact_num(struct Bookings* booking) {
	printf("Enter Contact Number: ");
	scanf("%14s", booking->contact_num);
} 

void fetch_date(struct Bookings *booking) {
	time_t tnull = time(NULL);
	struct tm current_date = *localtime(&tnull);
	struct tm new_date = { .tm_year = current_date.tm_year, .tm_mon = current_date.tm_mon, .tm_mday = current_date.tm_mday + booking->DOS };
	mktime(&new_date);
	strftime(booking->BD, 20, "%x %I:%M%p", &current_date);
	strftime(booking->ND, 10, "%x", &new_date);
}

int get_email(struct Bookings *booking) {
	int found = 0;
	char input[64];
	int tries = 0;
	while (1) {
		if (tries == 3) return 1;
		printf("Enter Email-Address: ");
		scanf("%63s", &input);
		for (int i = 0; input[i] != '\0'; i++) {
			if (input[i] == '@') found = 1;
		}
		if (found == 0) {
			printf("Invalid Email Address, make sure to input a valid email.\n");
			tries++;
			continue;
		}
		else break;
	}
	strcpy(booking->email, input);
	return 0;
}

int get_DOS(struct Bookings *booking) {
	char input[4];
	int int_input;
	int tries = 0;
	while (1) {
		if (tries == 3) return 1;
		printf("Enter desired days of stay: ");
		scanf("%3s", &input);
		int_input = atoi(input); // Convert String to Int, if no digits are detected the value is 0.
		if ((int_input > 0) && (int_input < 365)) break; // If atoi value is 0 (no digits) or below (negative number of days) we continue the loop, else we break.
		tries++;
		printf("Invalid Integer, please make sure to input a valid number of days more than 0 and less than 365.\n");
	}
	booking->DOS = int_input;
	return 0;
}

int change_room(struct Bookings* booking, struct Rooms* room) {
	FILE* ptrR = fopen("Rooms.txt", "r");
	char line[40],  lines[NUMBER_OF_ROOMS][40], line_args[4][20], *pline = 000000;
	int num = 0, count = 0, current_line_num, line_nums[NUMBER_OF_ROOMS], input;
	while (fgets(line, 40, ptrR)) { 
		strcpy(lines[num], line);
		line[strcspn(line, "\n")] = 0;
		pline = strtok(line, delimiter);
		if (strncmp(booking->ID, pline, 3) == 0) current_line_num = num;
		for (int i = 0; pline != NULL; i++) {
			strcpy(line_args[i], pline);
			pline = strtok(NULL, delimiter);
		}
		if (strncmp(line_args[3], "OPEN", 4) == 0) {
			line_nums[count] = num;
			count++;
			printf("(%2d) ID: %s || Type: %-15s || Price: %-4s Ringgit/Day\n", count, line_args[0], line_args[1], line_args[2]);
		}
		num++;
	}
	if (count == 0) {
		printf("There are no Available Rooms.\n");
		return 1;
	}
	printf("===========================================\n");
	fclose(ptrR);
	printf("Enter number of room to change to: ");
	scanf("%d", &input);
	if (input == 0) return 1;
	if (input < 0 || input > count) {
		printf("Invalid Number, make sure to input a number from the list\n");
		return 1;
	}
	num = line_nums[input - 1];
	strncpy(line, lines[num], 40);
	pline = strtok(line, delimiter);
	for (int j = 0; j <= 2; j++) {
		strcpy(line_args[j], pline);
		pline = strtok(NULL, delimiter);
	}
	snprintf(lines[num], 40, "%s||%s||%s||OCCUPIED\n", line_args[0], line_args[1], line_args[2]);
	snprintf(lines[current_line_num], 40, "%s||%s||%d||OPEN\n", room->ID, room->type, room->price);
	strncpy(room->ID, line_args[0], 4);
	strncpy(room->type, line_args[0], 20);
	room->price = atoi(line_args[2]);
	strncpy(booking->ID, room->ID, 4);
	update_rooms(lines);
	return 0;
}

void change_DOS(struct Bookings* booking) {
	char date[10];
	char* pdate = 000000;
	Date b_date;
	strncpy(date, booking->BD, 10);
	struct tm new_date = { 0 };
	pdate = strtok(date, "/");
	b_date.month = atoi(pdate);
	pdate = strtok(NULL, "/");
	b_date.day = atoi(pdate);
	pdate = strtok(NULL, " ");
	b_date.year = atoi(pdate);
	new_date.tm_year += b_date.year + 100;
	new_date.tm_mon += b_date.month - 1;
	new_date.tm_mday += b_date.day + booking->DOS;
	mktime(&new_date);
	strftime(booking->ND, 10, "%x", &new_date);
}

void display_booking(int mode) {
	FILE* ptrB = fopen("Bookings.txt", "r");
	Booking b;
	int count = 0;
	char line[205], *pline = 000000, status[9];
	switch (mode) {
	case 2: strncpy(status, "ACTIVE", 9); break;
	case 3: strncpy(status, "OUTDATED", 9); break;
	}
	printf("===========================================\n");
	if (mode == 1) {
		printf("\tAll Booking Records\n");
		printf("===========================================\n");
		while (fgets(line, 205, ptrB)) {
			count++;
			pline = strtok(line, delimiter);
			strncpy(b.passportIC, pline, 20);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			strncpy(b.ND, pline, 10);
			pline = strtok(NULL, delimiter);
			strncpy(b.ID, pline, 4);
			pline = strtok(NULL, delimiter);
			strncpy(b.name, pline, 40);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			strncpy(b.BD, pline, 20);
			b.BD[strcspn(b.BD, "\n")] = 0;
			printf("(%d) Pass/IC: %-20s || Name: %-20s\n    Begin Date:  %s || End Date: %s\n    Room: %s\n", count, b.passportIC, b.name, b.BD, b.ND, b.ID);
			printf("-------------------------------------------\n");
		}
	}
	else {
		printf("\t%s Booking Records\n", status);
		printf("===========================================\n");
		while (fgets(line, 205, ptrB)) {
			pline = strtok(line, delimiter);
			strncpy(b.passportIC, pline, 20);
			pline = strtok(NULL, delimiter);
			if (strncmp(pline, status, 9) != 0) continue;
			count++;
			pline = strtok(NULL, delimiter);
			strncpy(b.ND, pline, 10);
			pline = strtok(NULL, delimiter);
			strncpy(b.ID, pline, 4);
			pline = strtok(NULL, delimiter);
			strncpy(b.name, pline, 40);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			pline = strtok(NULL, delimiter);
			strncpy(b.BD, pline, 20);
			b.BD[strcspn(b.BD, "\n")] = 0;
			printf("(%d) Pass/IC: %-20s || Name: %-20s\n    Begin Date:  %s || End Date: %s\n    Room: %s\n", count, b.passportIC, b.name, b.BD, b.ND, b.ID);
			printf("-------------------------------------------\n");
		}
	}
	printf("===========================================\n");
	printf("           Number of records: %d           \n", count);
	printf("===========================================\n");
	fclose(ptrB);
	printf("Press any key to continue...\n");
	_getch();
}

void view_bookings() {
	int menu_choice;
	printf("===========================================\n");
	printf("                View Bookings              \n");
	printf("===========================================\n");
	printf("(1) View All Booking Records\n");
	printf("(2) View Active Booking Records\n");
	printf("(3) View Completed Booking Records\n");
	printf("(0) Return to main menu.\n");
	printf("===========================================\n");
	printf("Enter an Integer: ");
	scanf("%d", &menu_choice);
	switch (menu_choice) {
		case 0: return main_menu();
		case 1: display_booking(1); break;
		case 2: display_booking(2); break;
		case 3: display_booking(3); break;
		default: {
			printf("Please pick a valid menu choice.\n");
			while (getchar() != '\n');
			return view_bookings();
		}
	}
	return main_menu();
}

int confirm_booking(struct Bookings* booking) {
	char input;
	printf("=====================================\n");
	printf("               CONFIRM               \n");
	printf("=====================================\n");
	printf("Client Name    : %s\nPass/IC Number : %s\n", booking->name, booking->passportIC);
	printf("Email          : %s\nContact        : %s\n", booking->email, booking->contact_num);
	printf("Days of Stay   : %d\n", booking->DOS);
	printf("=====================================\n");
	printf("Would you like to proceed? 'Y' to proceed: ");
	input = getchar();
	while ((getchar()) != '\n');
	if (input == 'Y' || input == 'y') return 0;
	return 1;
}

void modify_details(struct Bookings* booking, struct Rooms* room, int line_num) {
	int modify_choice;
	printf("===========================================\n");
	printf("            Booking Modification           \n");
	printf("===========================================\n");
	printf("(0) Return to main menu.\n");
	printf("(1) Change Room\n");
	printf("(2) Change E-Mail\n");
	printf("(3) Change Contact Number\n");
	printf("(4) Change Days Of Stay\n");
	printf("===========================================\n");
	printf("Enter menu choice: ");
	scanf("%d", &modify_choice);
	printf("===========================================\n");
	switch (modify_choice) {
	case 0: return main_menu();
	case 1:
		if (change_room(booking, room) == 0) {
			printf("Room has been successfully changed!\n");
		}
		else {
			printf("Room was not changed, returning to menu\n");
			return modify_details(booking, room, line_num);
		}
		break;
	case 2:
		if (get_email(booking) == 0){
			printf("Email has been successfully changed!\n");
		}
		else {
			printf("Email was not changed, returning to menu.");
			return modify_details(booking, room, line_num);
		}
		break;
	case 3:
		get_contact_num(booking);
		printf("Contact Nubmer has been successfully changed!\n");
		break;
	case 4:
		if (get_DOS(booking) == 0) {
			change_DOS(booking);
			printf("Days of stay have been modified!\n");
		}
		else {
			printf("Days of stay was not changed, returning to menu");
			return modify_details(booking, room, line_num);
		}
		break;
	default:
		printf("Please pick a valid menu choice.\n");
		while (getchar() != '\n');
		main_menu();
	}
	update_record(booking, line_num);
}

void generate_receipt(struct Bookings *booking, struct Rooms *room) {
	// This function is responsible for generating receipt for a chosen record.
	float service_tax = 0, tourism_tax = 0, total_price = 0;
	struct tm current_time;
	time_t t = time(NULL);
	printf("===========================================\n");
	service_tax = ((booking->DOS) * (room->price) * SERVICE_TAX);
	tourism_tax = ((booking->DOS) * TOURISM_TAX);
	total_price = service_tax + tourism_tax + ((booking->DOS) * (room->price));
	current_time = *localtime(&t);
	printf("Name: %s\n", booking->name);
	printf("RoomID: %s\nPrice Per Night: %d Ringgit/Night\n", room->ID, room->price);
	printf("Days Of Stay: %d Days\nService Tax: %.2f Ringgit\n", booking->DOS, service_tax);
	printf("Tourism Tax : %.2f Ringgit\nTotal Price : %.2f Ringgit\n", tourism_tax, total_price);
	printf("Receipt Generated At: %s", asctime(&current_time));
	printf("===========================================\n");
	printf("Press any key to return to menu....\n");
	_getch();
}

void book() {
	Booking new_booking;
	FILE *ptrR = fopen("Rooms.txt", "r");
	if (ptrR == NULL) {
		printf("Unable to access Rooms.txt file, make sure file exists.\m");
		return main_menu();
	}
	// Room Details
	char* pline = 000000, line[40], lines[NUMBER_OF_ROOMS][40], line_args[4][20];
	int num = 0, line_nums[NUMBER_OF_ROOMS], count = 0, input;
	printf("===========================================\n");
	printf("           Available Hotel Rooms           \n");
	printf("===========================================\n\n");
	while (fgets(line, 40, ptrR)) { // Fetch every line from Rooms.txt
		//do the initial checking and print the checked results.
		strcpy(lines[num], line); // Store the line in an array for future file editing.
		line[strcspn(line, "\n")] = 0; // Strip the trailing newline at the end, if it exists
		pline = strtok(line, delimiter); // Split the line into arguments in this order: ID||TYPE||PRICE||STATUS
		for (int i = 0; pline != NULL; i++) {
			strcpy(line_args[i], pline);
			pline = strtok(NULL, delimiter);
		}
		if (strncmp(line_args[3], "OPEN", 4) == 0) { // Check if the status of the room is OPEN meaning that it can be booked. If so, display the room as option for the user to pick.
			line_nums[count] = num;
			count++;
			printf("(%2d) ID: %s || Type: %-15s || Price: %-4s Ringgit/Day || Status: %s\n", count, line_args[0], line_args[1], line_args[2], line_args[3]);
		}
		num++;
	}
	if (count == 0) {
		printf("There are no available rooms.\n");
		return main_menu();
	}
	printf("\n===========================================\n");
	fclose(ptrR);
	printf("Enter number of room for booking, '0' to return to menu: ");
	scanf("%d", &input);
	if (input == 0) return main_menu(); // If user inputs 0, they are able to cancel the booking process and return to main menu.
	if (input < 0 || input > count) {
		printf("Invalid Number, make sure to input a number from the list\n");
		return main_menu();
	}
	while (getchar() != '\n');
	get_name(&new_booking);
	get_passport_ic(&new_booking);
	while ((getchar()) != '\n');
	if (check_viable(new_booking.passportIC) == 0) {
		printf("You already have one active booking registered to this Passport/IC Number.\n");
		return main_menu();
	}
	if (get_email(&new_booking) == 1) {
		while ((getchar()) != '\n');
		printf("Returning to main menu\n");
		return main_menu();
	}
	get_contact_num(&new_booking);
	while ((getchar()) != '\n');
	if (get_DOS(&new_booking) == 1) {
		while ((getchar()) != '\n');
		printf("Returning to main menu\n");
		return main_menu();
	}
	while ((getchar()) != '\n');
	if ((confirm_booking(&new_booking)) == 1)
	{
		printf("Booking process has been terminated!\n");
		return main_menu();
	}
	fetch_date(&new_booking);
	num = line_nums[input - 1]; // Storing the chosen room's line number in the num variable.
	pline = strtok(lines[num], delimiter); // Fetching the RoomID from line.
	strncpy(new_booking.ID, pline, 4); // Fetching RoomID
	strncpy(new_booking.status, "ACTIVE", 9); // Setting Booking status to ACTIVE
	add_booking(&new_booking); // Adding the booking record to Bookings.txt through the function add_booking()
	//// Updating Room record in Rooms.txt to be Occupied.
	for (int j = 0; j <= 2; j++) {
		strcpy(line_args[j], pline);
		pline = strtok(NULL, delimiter);
	}
	snprintf(lines[num], 40, "%s||%s||%s||OCCUPIED\n", line_args[0], line_args[1], line_args[2]);
	update_rooms(lines);
	printf("Booking has been successfully processed!\n");
	main_menu();
} 

void booking_details() {
	Booking booking[5], *ptrR, *ptrB;
	Room room[5];
	char pass_ic[20], input;
	int num_found, chosen, line_nums[5];
	printf("===========================================\n");
	printf("               Booking Details             \n");
	printf("===========================================\n");
	printf("Input Passport or IC Number linked to booking: ");
	scanf("%s", &pass_ic);
	if ((strlen(pass_ic) <= 2) && (pass_ic[0] == '0')) return main_menu();
	num_found = fetch_details(pass_ic, booking, room, line_nums);
	printf("===========================================\n");
	if (num_found == 0) {
		printf("No active booking has been found linked to given Passport/IC Number, returning to menu.\n");
		return main_menu();
	}
	else if (num_found == 1) {
		chosen = 0;
	}
	else {
		for (int i = 0; i < num_found; i++) {
			printf("(%d) Passport/IC: %s || RoomID: %-3s || StartDate: %s || EndDate: %s || Status: %s\n", i + 1, pass_ic, booking[i].ID, booking[i].BD, booking[i].ND, booking[i].status);
		}
		printf("===========================================\n");
		printf("Enter number of booking record, '0' to return to main menu: ");
		while ((getchar()) != '\n');
		input = getchar();
		if ((isdigit(input)) == 0) {
			printf("Make sure to input a valid number form the list\n");
			while ((getchar()) != '\n');
			return booking_details();
		}
		chosen = input - '0';
		if (chosen == 0) return main_menu();
		if (chosen > num_found) {
			printf("Make sure to input a valid number from the list\n");
			while ((getchar()) != '\n');
			return booking_details();
		}
		chosen--;
		printf("===========================================\n");
	}
	printf("Room ID      : %s\nRoom Type    : %s\n", room[chosen].ID, room[chosen].type);
	printf("Client       : %s\nPassport/IC  : %s\n", booking[chosen].name, booking[chosen].passportIC);
	printf("Email        : %s\nContact      : %s\n", booking[chosen].email, booking[chosen].contact_num);
	printf("Days of Stay : %d\nBooking Date : %s\nEnd Date     : %s\n", booking[chosen].DOS, booking[chosen].BD, booking[chosen].ND);
	printf("Status       : %s\n", booking[chosen].status);
	printf("===========================================\n");
	if (strncmp(booking[chosen].status, "ACTIVE", 6) == 0) {
		printf("(0) Return to main menu.\n");
		printf("(1) Generate Receipt For Record.\n");
		printf("(2) Modify Booking Details\n");
		printf("===========================================\n");
		printf("Input choice of action: ");
		while ((getchar()) != '\n');
		input = getchar();
		switch (input) {
		case('0'): return main_menu();
		case('1'): generate_receipt(&booking[chosen], &room[chosen]); return main_menu();
		case('2'): modify_details(&booking[chosen], &room[chosen], line_nums[chosen]); break;
		default: printf("Make sure to input a valid number from the list of actions!.\n"); return booking_details();
		printf("===========================================\n");
		}
	}
	printf("Would you like to generate receipt for chosen record? Y/N : ");
	while ((getchar()) != '\n');
	input = getchar();
	while ((getchar()) != '\n');
	if (!(input == 'y' || input == 'Y')) return main_menu();
	generate_receipt(&booking[chosen], &room[chosen]);
	/*generate_receipt(booking, room[chosen]);*/
	return main_menu();
}

void main_menu() {
	int menu_choice;
	printf("===========================================\n");
	printf("         Hotel Room Booking System         \n");
	printf("===========================================\n");
	printf("(1) View Hotel Rooms\n");
	printf("(2) Book a Room\n");
	printf("(3) Search and Modify Bookings and View Receipt\n");
	printf("(4) View Booking Records\n");
	printf("(0) Exit Program\n");
	printf("===========================================\n");
	printf("Enter an Integer: ");
	scanf("%d", &menu_choice);
	switch(menu_choice){
		case 0: exit(0); break;
		case 1: view_rooms(); break;
		case 2: book(); break;
		case 3: booking_details(); break;
		case 4: view_bookings(); break;
		default: {
			printf("Please pick a valid menu choice.\n"); 
			while (getchar() != '\n');
			main_menu();
		}
	}
}

void main() {
	check_occupation();
	main_menu();
}

