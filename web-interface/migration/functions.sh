ask_yn_question()
{
	QUESTION=$1

	while true;
	do
		echo -n "${QUESTION} (y/n) "
		read REPLY
		if [ "${REPLY}" == "y" ];
		then
			return 0;
		fi
		if [ "${REPLY}" == "n" ];
		then
			return 1;
		fi
		echo "Don't tell ya life, reply using 'y' or 'n' "'!'
	done
}
