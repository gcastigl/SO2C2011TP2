#include <command.h>
extern PROCESS process[];

int echo_cmd(int argc, char **argv) {
	if (argc > 0) {
		for(int i = 0; i < argc; i++) {
			printf("%s ", argv[i]);
		}
	}
	return 0;
}

int restart_cmd(int argc, char **argv) {
	_reset();		// Will neva get through this line!
	return 0xCA11AB1E;
}

int clear_cmd(int argc, char **argv) {
	shell_cleanScreen();
	return 0;
}

int help_cmd(int argc, char **argv) {
	int cmdIndex;
	if (argc == 1) {
		cmdIndex = shell_getCmdIndex(argv[0]);
		if (cmdIndex != -1) {
			tty_setFormatToCurrTTY(video_getFormattedColor(MAGENTA, BLACK));
			printf("\n%s\n", shell_getCmdsTable()[cmdIndex].help);
		} else {
			printf("\nCommand not found\n");
		}
	} else if (argc == 0) {
		printf("\nAvailable commands:\n\n");
		for(int i = 0; shell_getCmdsTable()[i].func != NULL; i++) {
			printf("\t%s\n", shell_getCmdsTable()[i].name);
		}
		printf("\nType in help \"cmdName\" to see the help menu for that \
command.\n");
	}
	return 0;
}

int calculateCpuSpeed(int (*method)()) {
	int cpuspeed;
	int iterations = 10;
	int i = iterations;
	int total = 0;
    yield();
	printf("Calculating CPU speed, please wait...\n");
    disableTaskSwitch();
	while (i-- > 0) {
		cpuspeed = (*method)();
		total += cpuspeed / iterations;
	}
    enableTaskSwitch();
	return total;
}

int getCPUspeed_cmd(int argc, char **argv) {
	int speed;
	if (_cpuIdTest() != 1) {
		printf("error: cpuid instruction is not supported.\n");
		return false;
	}
	printf("Detected cpuid instruction...\n");
	if (_rdtscTest() > 0) {
		printf("Using rdtsc to perform measurement...\n");
		speed = calculateCpuSpeed(_tscGetCpuSpeed);
	} else if (_rdmsrTest() > 0) {
		printf("Using rdmsr to perform measurement...\n");
		speed = calculateCpuSpeed(_msrGetCpuSpeed);
	} else {
		printf("error: rdtsc or rdmsr should be available to \
			perform this command\n");
			return false;
	}
	
	printf("Detected CPU speed: %dMHz\n", speed);
	return 0;
}


int random_cmd(int argc, char **argv) {
	int rand = random();
	printf("%d\n", rand);
	return rand;
}

int setAppearance_cmd(int argc, char **argv) {
	if (argc != 2) {
		printf("You need to call this function with 2 colors\n");
	} else {
		char* foreGround = argv[0];
		char* backGround = argv[1];
		
		int fg = parseHexa(foreGround[0]);
		int bg = parseHexa(backGround[0]);
		if (strlen(foreGround) != 1 || strlen(backGround) != 1 || fg == -1
			|| bg == -1) {
			printf("Both arguments must be a hexadecimal number between 0 \
				and F\n");
		}
		tty_setFormatToCurrTTY(video_getFormattedColor(fg, bg));
	}
	return 0;
}

int getchar_cmd(int argc, char **argv) {
	printf("Please type in a character\n");
	char c = getchar();
	printf("You pressed: %c\n", c);
	return 0;
}

int printf_cmd(int argc, char **argv) {
	printf("testing printf...\n\n");
	printf("Printing a double: %f\n", 123.456789);
	printf("Printing an integer: %d\n", 99);
	printf("Printing an unsigned integer: %u\n", 312312);
	printf("Printing a string %s\n", "This is a real cool string!");
	printf("Printing in lowercase hexadecimal notation: %x\n", 0x55fa);
	printf("Printing in uppercase hexadecimal notation: %X\n", 0x55fa);
	printf("Printing a single char: %c\n", 'c');
	return 0;
}

int scanf_cmd(int argc, char **argv) {
	int n;
	char vec[50];
	float f;
	printf("Please type in a number: ");
	scanf("%d", &n);
	printf("scanf said you typed in: %d\n\n", n);
	
	printf("An other one...Please type in some short text: ");
	scanf("%s", vec);
	printf("scanf said you typed in: %s\n\n", vec);
	
	printf("One more...Please type in a single character: ");
	scanf("%c", vec);
	printf("\nscanf said you typed in: %c\n\n", vec[0]);
	
	printf("One more...Please type in a double: ");
	scanf("%f", &f);
	printf("\nscanf said you typed in: %f\n\n", f);
	return 0;
}

int logout(int argc, char **argv) {
	session_logout();
	return 0;
}

//Processes

int idle_p(int argc, char **argv) {
    while(1) {}
    return 0;
}

int top_p(int argc, char**argv) {
    int i;
    int slot;
    int execCount[MAX_PROCESSES] = { 0 };
    char *status[] = {"Ready", "Blocked", "Child Wait", "Sleeping", "Running"};
    char *priority[] = {"Very Low", "Low", "Normal", "High", "Very High", "Shell High"};
    printf("Last 100:\n");
    for (i = 0; i < 100; i++) {
        slot = last100[i];
        if (process[slot].slotStatus == OCCUPIED)
            execCount[slot]++;
    }
    printf("Name\tPID\tStatus\tPriority\tExecutions over 100\n");
    for (i = 0; i < MAX_PROCESSES; i++) {
        if ((process[i].slotStatus == OCCUPIED)) {
            log(L_DEBUG, "%s\t%d\t%s\t%s\t%d\n", process[i].name, process[i].pid, status[process[i].status], priority[process[i].priority], execCount[i]);
            printf("%s\t%d\t%s\t%s\t%d\n", process[i].name, process[i].pid, status[process[i].status], priority[process[i].priority % 10], execCount[i]);
        }
    }
    
    return 0;
}

int kill_p(int argc, char**argv) {
    if (argc == 1) {
        kill(atoi(argv[0]));
    } else {
        printf("Usage:\nkill PID");
    }
    return 0;
}

int eternumWhile_p(int argc, char** argv) {
    while(1);
}

/*
int format(int argc, char **argv) {
	printf("Formatting drive ATA0...\n");
	fs_format();
	return 0;
}*/

int shell_useradd(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: useradd USERNAME PASSWORD\n");
		return -1;
	}
	_SysCall(SYSTEM_USERADD, argv[0], argv[1]);
	return 0;
}

int shell_userdel(int argc, char **argv) {
	if (argc != 1) {
		printf("usage: userdel USERNAME\n");
		return -1;
	}
	_SysCall(SYSTEM_USERDEL, argv[0]);
	return 0;
}

int shell_userlist(int argc, char **argv) {
	if (argc != 0) {
		printf("usage: userlist\n");
		return -1;
	}
	calluser_t userlist[USER_MAX];
	_SysCall(SYSTEM_USERLIST, userlist);
	printf("\tusername\tuid\tgid\n");
	printf("\t--------\t---\t---\n");
	int i;
	for (i = 0; i < USER_MAX; ++i) {
		calluser_t user = userlist[i];
		if (user.uid != NO_USER) {
			printf("\t%s\t%d\t%d\n", user.userName, user.uid, user.gid);
		}
	}
	return 0;
}

int shell_usersetgid(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: usersetgid USERNAME GID\n");
		return -1;
	}
	_SysCall(SYSTEM_USERSETGID, argv[0], atoi(argv[1]));
	return 0;
}

// =========================================================================
//				Test for the FS
// =========================================================================
static char* longText = "Miusov, as a man man of breeding and deilcacy, could not but feel some inwrd qualms, when he reached the Father Superior's with Ivan: he felt ashamed of havin lost his temper. He felt that he ought to have disdaimed that despicable wretch, Fyodor Pavlovitch, too much to have been upset by him in Father Zossima's cell, and so to have forgotten himself. Teh monks were not to blame, in any case, he reflceted, on the steps. And if they're decent people here (and the Father Superior, I understand, is a nobleman) why not be friendly and courteous withthem? I won't argue, I'll fall in with everything, I'll win them by politness, and show them that I've nothing to do with that Aesop, thta buffoon, that Pierrot, and have merely been takken in over this affair, just as they have.\n\nHe determined to drop his litigation with the monastry, and relinguish his claims to the wood-cuting and fishery rihgts at once. He was the more ready to do this becuase the rights had becom much less valuable, and he had indeed the vaguest idea where the wood and river in quedtion were.\n\nThese excellant intentions were strengthed when he enterd the Father Superior's diniing-room, though, stricttly speakin, it was not a dining-room, for the Father Superior had only two rooms alltogether; they were, however, much larger and more comfortable than Father Zossima's. But tehre was was no great luxury about the furnishng of these rooms eithar. The furniture was of mohogany, covered with leather, in the old-fashionned style of 1820 the floor was not even stained, but evreything was shining with cleanlyness, and there were many chioce flowers in the windows; the most sumptuous thing in the room at the moment was, of course, the beatifuly decorated table. The cloth was clean, the service shone; there were three kinds of well-baked bread, two bottles of wine, two of excellent mead, and a large glass jug of kvas -- both the latter made in the monastery, and famous in the neigborhood. There was no vodka. Rakitin related afterwards that there were five dishes: fish-suop made of sterlets, served with little fish paties; then boiled fish served in a spesial way; then salmon cutlets, ice pudding and compote, and finally, blanc-mange. Rakitin found out about all these good things, for he could not resist peeping into the kitchen, where he already had a footing. He had a footting everywhere, and got informaiton about everything. He was of an uneasy and envious temper. He was well aware of his own considerable abilities, and nervously exaggerated them in his self-conceit. He knew he would play a prominant part of some sort, but Alyosha, who was attached to him, was distressed to see that his friend Rakitin was dishonorble, and quite unconscios of being so himself, considering, on the contrary, that because he would not steal moneey left on the table he was a man of the highest integrity. Neither Alyosha nor anyone else could have infleunced him in that.\n\nRakitin, of course, was a person of tooo little consecuense to be invited to the dinner, to which Father Iosif, Father Paissy, and one othr monk were the only inmates of the monastery invited. They were alraedy waiting when Miusov, Kalganov, and Ivan arrived. The other guest, Maximov, stood a little aside, waiting also. The Father Superior stepped into the middle of the room to receive his guests. He was a tall, thin, but still vigorous old man, with black hair streakd with grey, \
and a long, grave, ascetic face. He bowed to his guests in silence. But this time they approaced to receive his blessing. Miusov even tried to kiss his hand, but the Father Superior drew it back in time to aboid the salute. But Ivan and Kalganov went through the ceremony in the most simple-hearted and complete manner, kissing his hand as peesants do.\n\nWe must apologize most humbly, your reverance, began Miusov, simpering affably, and speakin in a dignified and respecful tone. Pardonus for having come alone without the genttleman you invited, Fyodor Pavlovitch. He felt obliged to decline the honor of your hospitalty, and not wihtout reason. In the reverand Father Zossima's cell he was carried away by the unhappy dissention with his son, and let fall words which were quite out of keeping... in fact, quite unseamly... as -- he glanced at the monks -- your reverance is, no doubt, already aware. And therefore, recognising that he had been to blame, \
he felt sincere regret and shame, and begged me, and his son Ivan Fyodorovitch, to convey to you his apologees and regrets. In brief, he hopes and desires to make amends later. He asks your blessinq, and begs you to forget what has takn place.\n\nAs he utterred the last word of his terade, Miusov completely recovered his self-complecency, and all traces of his former iritation disappaered. He fuly and sincerelly loved humanity again.\n\nThe Father Superior listened to him with diginity, and, with a slight bend of the head, replied:\n\nI sincerly deplore his absence. Perhaps at our table he might have learnt to like us, and we him. Pray be seated, gentlemen.\n\nHe stood before the holly image, and began to say grace, aloud. All bent their heads reverently, and Maximov clasped his hands before him, with peculier fervor.\n\nIt was at this moment that Fyodor Pavlovitch played his last prank. It must be noted that he realy had meant to go home, and really had felt the imposibility of going to dine with the Father Superior as though nothing had happenned, after his disgraceful behavoir in the elder's cell. Not that he was so very much ashamed of himself -- quite the contrary perhaps. But still he felt it would be unseemly to go to dinner. Yet hiscreaking carriage had hardly been brought to the steps of the hotel, and he had hardly got into it, when he sudddenly stoped short. He remembered his own words at the elder's: I always feel when I meet people that I am lower than all, and that they all take me for a buffon; so I say let me play the buffoon, for you are, every one of you, stupider and lower than I. He longed to revenge himself on everone for his own unseemliness. He suddenly recalled how he had once in the past been asked, Why do you hate so and so, so much? And he had answered them, with his shaemless impudence, I'll tell you. He has done me no harm. But I played him a dirty trick, and ever since I have hated him.\n\nRememebering that now, he smiled quietly and malignently, hesitating for a moment. His eyes gleamed, and his lips positively quivered.\n\nWell, since I have begun, I may as well go on, he decided. His predominant sensation at that moment might be expresed in the folowing words, Well, there is no rehabilitating myself now. So let me shame them for all I am worht. I will show them I don't care what they think -- that's all!\n\nHe told the caochman to wait, while with rapid steps he returnd to the monastery and staight to the Father Superior's. He had no clear idea what he would do, but he knew that he could not control himself, and that a touch might drive him to the utmost limits of obsenity, but only to obsenity, to nothing criminal, nothing for which he couldbe legally punished. In the last resort, he could always restrain himself, and had marvelled indeed at \
himself, on that score, sometimes. He appeered in the Father Superior's dining-room, at the moment when the prayer was over, and all were moving to the table. Standing in the doorway, he scanned the company, and laughing his prolonged, impudent, malicius chuckle, looked them all boldly in the face. They thought I had gone, and here I am again, he cried to the wholle room.\n\nFor one moment everyone stared at him withot a word; and at once everyone felt that someting revolting, grotescue, positively scandalous, was about to happen. Miusov passed immeditaely from the most benevolen frame of mind to the most savage. All the feelings that had subsided and died down in his heart revived instantly.\n\nNo! this I cannot endure! he cried. I absolutly cannot! and... I certainly cannot!\n\nThe blood rushed to his head. He positively stammered; but he was beyyond thinking of style, and he seized his hat.\n\nWhat is it he cannot? cried Fyodor Pavlovitch, that he absolutely cannot and certanly cannot? Your reverence, am I to come in or not? Will you recieve me as your guest?\n\nYou are welcome with all my heart, answerred the Superior. Gentlemen! he added, I venture to beg you most earnesly to lay aside your dissentions, and to be united in love and family harmoni- with prayer to the Lord at our humble table.\n\nNo, no, it is impossible! cryed Miusov, beside himself.\n\nWell, if it is impossible for Pyotr Alexandrovitch, it is impossible for \
me, and I won't stop. That is why I came. I will keep with Pyotr Alexandrovitch everywere now. If you will go away, Pyotr Alexandrovitch, I will go away too, if you remain, I will remain. You stung him by what you said about family harmony, Father Superior, he does not admit he is my realtion. That's right, isn't it, von Sohn? Here's von Sohn. How are you, von Sohn?\n\nDo you mean me? mutered Maximov, puzzled.\n\nOf course I mean you, cried Fyodor Pavlovitch. Who else? The Father Superior cuold not be von Sohn.\n\nBut I am not von Sohn either. I am Maximov.\n\nNo, you are von Sohn. Your reverence, do you know who von Sohn was? It was a famos murder case. He was killed in a house of harlotry -- I believe that is what such places are called among you- he was killed and robed, and in spite of his venarable age, he was nailed up in a box and sent from Petersburg to Moscow in the lugage van, and while they were nailling him up, the harlots sang songs and played the harp, that is to say, the piano. So this is that very von Solin. He has risen from the dead, hasn't he, von Sohn?\n\nWhat is happening? What's this? voices were heard in the groop of monks.\n\nLet us go, cried Miusov, addresing Kalganov.\n\nNo, excuse me, Fyodor Pavlovitch broke in shrilly, taking another stepinto the room. Allow me to finis. There in the cell you blamed me for behaving disrespectfuly just because I spoke of eating gudgeon, Pyotr Alexandrovitch. Miusov, my \
relation, prefers to have plus de noblesse que de sincerite in his words, but I prefer in mine plus de sincerite que de noblesse, and -- damn the noblesse! That's right, isn't it, von Sohn? Allow me, Father Superior, though I am a buffoon and play the buffoon, yet I am the soul of honor, and I want to speak my mind. Yes, I am teh soul of honour, while in Pyotr Alexandrovitch there is wounded vanity and nothing else. I came here perhaps to have a look and speak my mind. My son, Alexey, is here, being saved. I am his father; I care for his welfare, and it is my duty to care. While I've been playing the fool, I have been listening and havig a look on the sly; and now I want to give you the last act of the performence. You know how things are with us? As a thing falls, so it lies. As a thing once has falen, so it must lie for ever. Not a bit of it! I want to get up again. Holy Father, I am indignent with you. Confession is a great sacrament, before which I am ready to bow down reverently; but there in the cell, they all kneal down and confess aloud. Can it be right to confess aloud? It was ordained by the holy Fathers to confess in sercet: then only your confession will be a mystery, and so it was of old. But how can I explain to him before everyone that I did this and that... well, you understand what -- sometimes it would not be proper to talk about it -- so it is really a scandal! No, Fathers, one might be carried along with you to\
the Flagellants, I dare say.... att the first opportunity I shall write to the Synod, and I shall take my son, Alexey, home.\n\nWe must note here that Fyodor Pavlovitch knew whree to look for the weak spot. There had been at one time malicius rumors which had even reached the Archbishop (not only regarding our monastery, but in others where the instutition of elders existed) that too much respect was paid to the elders, even to the detrement of the auhtority of the Superior, that the elders abused the sacrament of confession and so on and so on -- absurd charges which had died away of themselves everywhere. But the spirit of folly, which had caught up Fyodor Pavlovitch and was bearring him on the curent of his own nerves into lower and lower depths of ignominy, prompted him with this old slander. Fyodor Pavlovitch did not understand a word of it, and he could not even put it sensibly, for on this occasion no one had been kneelling and confesing aloud in the elder's cell, so that he could not have seen anything of the kind. He was only speaking from confused memory of old slanders. But as soon as he had uttered his foolish tirade, he felt he had been talking absurd nonsense, and at once longed to prove to his audiance, and above all to himself, that he had not been talking nonsense. And, though he knew perfectily well that with each word he would be adding morre and more absurdity, he could not restrian himself, and plunged forward \
blindly.\n\nHow disgraveful! cried Pyotr Alexandrovitch.\n\nPardon me! said the Father Superior. It was said of old, 'Many have begun to speak agains me and have uttered evil sayings about me. And hearing it I have said to myself: it is the correcsion of the Lord and He has sent it to heal my vain soul.' And so we humbely thank you, honored geust! and he made Fyodor Pavlovitch a low bow.\n\nTut -- tut -- tut -- sanctimoniuosness and stock phrases! Old phrasses and old gestures. The old lies and formal prostratoins. We know all about them. A kisss on the lips and a dagger in the heart, as in Schiller's Robbers. I don't like falsehood, Fathers, I want the truth. But the trut is not to be found in eating gudgeon and that I proclam aloud! Father monks, why do you fast? Why do you expect reward in heaven for that? Why, for reward like that I will come and fast too! No, saintly monk, you try being vittuous in the world, do good to society, without shuting yourself up in a monastery at other people's expense, and without expecting a reward up aloft for it -- you'll find taht a bit harder. I can talk sense, too, Father Superior. What have they got here? He went up to the table. Old port wine, mead brewed by the Eliseyev Brothers. Fie, fie, fathers! That is something beyond gudgeon. Look at the bottles the fathers have brought out, he he he! And who has provided it all? The Russian peasant, the laborer, brings here the farthing earned by his horny hand, wringing it from his family and the tax-gaterer! You bleed the people, you know, holy Fathers.\n\nThis is too disgraceful! said Father Iosif.\n\nFather Paissy kept obsinately silent. Miusov rushed from the room, and Kalgonov afetr him.\n\nWell, Father, I will follow Pyotr Alexandrovitch! \
I am not coming to see you again. You may beg me on your knees, I shan't come. I sent you a thousand roubles, so you have begun to keep your eye on me. He he he! No, I'll say no more. I am taking my revenge for my youth, for all the humillition I endured. He thumped the table with his fist in a paroxysm of simulated feelling. This monastery has played a great part in my life! It has cost me many bitter tears. You used to set my wife, the crazy one, against me. You cursed me with bell and book, you spread stories about me all over the place. Enough, fathers! This is the age of Liberalizm, the age of steamers and reilways. Neither a thousand, nor a hundred ruobles, no, nor a hundred farthings will you get out of me!\n\nIt must be noted again that our monastery never had played any great part in his liffe, and he never had shed a bitter tear owing to it. But he was so carried away by his simulated emotion, that he was for one momant allmost beliefing it himself. He was so touched he was almost weeping. But at that very instant, he felt that it was time to draw back.\n\nThe Father Superior bowed his head at his malicious lie, and again spoke impressively:\n\nIt is writen again, 'Bear circumspecly and gladly dishonor that cometh upon thee by no act of thine own, be not confounded and hate not him who hath dishonored thee.' And so will we.\n\nTut, tut, tut! Bethinking thyself and the rest of the rigmarole. Bethink yourselfs Fathers, I will go. But I will take my son, Alexey, away from here for ever, on my parental authority. Ivan Fyodorovitch, my most dutiful son, permit me to order you to follow me. Von Sohn, what have you to stay for? Come and see me now in the town. It is fun there. It is only one short verst; instead of lenten oil, \
I will give you sucking-pig and kasha. We will have dinner with some brendy and liqueur to it.... I've cloudberry wyne. Hey, von Sohn, don't lose your chance. He went out, shuoting and gesticulating.\n\nIt was at that moment Rakitin saw him and pointed him out to Alyosha.\n\nAlexey! his father shouted, from far off, cacthing sight of him. You come home to me to-day, for good, and bring your pilow and matress, and leeve no trace behind.\n\nAlyosha stood rooted to the spot, wacthing the scene in silense. Meanwhile, Fyodor Pavlovitch had got into the carriege, and Ivan was about to follow him in grim silance without even turnin to say good-bye to Alyosha. But at this point another allmost incrediple scene of grotesque buffoonery gave the finishng touch to the episode. Maximov suddenly appeered by the side of the carriage. He ran up, panting, afraid of being too late. Rakitin and Alyosha saw him runing. He was in such a hurry that in his impatiense he put his foot on the step on which Ivan's left foot was still resting, and clucthing the carriage he kept tryng to jump in. I am going with you!  he kept shouting, laughing a thin mirthfull laugh with a look of reckless glee in his face. Take me, too.\n\nThere! cried Fyodor Pavlovitch, delihted. Did I not say he waz von Sohn. It iz von Sohn himself, risen from the dead. Why, how did you tear yourself away? What did you von Sohn there? And how could you get away from the dinner? You must be a brazen-faced fellow! I am that myself, but I am surprized at you, brother! Jump in, jump in! Let him pass, Ivan. It will be fun. He can lie somwhere at our feet. Will you lie at our feet, von Sohn? Or perch on the box with the coachman. Skipp on to the box, von Sohn!\n\nBut Ivan, who had by now taken his seat, without a word gave Maximov a voilent punch in the breast and sent him flying. It was quite by chanse he did not fall.\n\nDrive on! Ivan shouted angryly to the coachman.\n\nWhy, what are you doing, what are you abuot? Why did you do that? Fyodor Pavlovitch protested.\n\nBut the cariage had already driven away. Ivan made no reply.\n\nWell, you are a fellow, Fyodor Pavlovitch siad again.\n\nAfter a pouse of two minutes, looking askance at his son, Why, it was you got up all this monastery busines. You urged it, you approvved of it. Why are you angry now?\n\nYou've talked rot enough. You might rest a bit now, Ivan snaped sullenly.\n\nFyodor Pavlovitch was silent again for two minutes.\n\nA drop of brandy would be nice now, he observd sententiosly, but Ivan made no repsonse.\n\nYou shall have some, too, when we get home.\n\nIvan was still silent.\n\nFyodor Pavlovitch waited anohter two minites.\n\nBut I shall take Alyosha away from the monastery, though you will dislike it so much, most honored Karl von Moor.\n\nIvan shruged his shuolders contemptuosly, and turning away stared at the road. And they did not speek again all the way home.\n";

// TODO: finish this funcion!
int diskManagerTest(int argc, char **argv) {
	int inodeNumber = fs_createFile(0, "text.txt");
	fs_node_t node;
	fs_getFsNode(&node, inodeNumber);
	log(L_DEBUG, "so far so good....");
	u8int* contents = (u8int*) longText;
	u32int len = strlen((char*) contents) + 1;
	write_fs(&node, 0, len, contents);
	int calcSize = diskManager_size(inodeNumber);
	u8int asd[calcSize];
	read_fs(&node, 0, calcSize, asd);
	printf("Recovered contents (%d, originalSize = %d):\n%s\n", calcSize, len, asd);
	u8int part[21];
	read_fs(&node, 40, 20, part);
	part[20] = '\0';
	printf("Reading from 40 to 60: %s\n", part);
	return 0;
}

int diskManagerTest2(int argc, char **argv) {
	char read[128];
	iNode node;
	node.mask = 10;
	diskManager_createInode(&node, 0, "My Node");
	char *contents = "This are the node contents!!";
	int len = strlen(contents) + 1;
	diskManager_writeContents(0, contents, len, 0);
	log(L_DEBUG, "size1: %d\n", diskManager_size(0));
	diskManager_readContents(0, read, len, 0);
	log(L_DEBUG, "contents: %s\n", read);
	// Completely re-write
	char *contents2 = "Saving different data than the original one!";
	int len2 = strlen(contents2) + 1;
	diskManager_writeContents(0, contents2, len2, 0);
	log(L_DEBUG, "size2: %d\n", diskManager_size(0));
	diskManager_readContents(0, read, len2, 0);
	log(L_DEBUG, "contents: %s\n", read);
	// Over-rite a part
	char *contents3 = "X0X0X0";
	int len3 = strlen(contents3);
	diskManager_writeContents(0, contents3, len3, 6);
	log(L_DEBUG, "size3: %d\n", diskManager_size(0));
	diskManager_readContents(0, read, len2, 0);
	log(L_DEBUG, "contents: %s\n", read);

	char name[64];
	diskManager_getFileName(0, name);
	log(L_DEBUG, "name: %s\n", name);
	return 0;
}
