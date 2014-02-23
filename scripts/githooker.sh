#!/bin/sh

#needs curl and git

lastfile=/tmp/githook.last
newfile=/tmp/githook.new
ircfile=/tmp/githook.irc
ircinp=/tmp/githook.inp

set -e
set -x

owner=srsbsns
repo=libsrsbsns

cd ~/src/${repo}

branch=$(git status | head -n1 | cut -d ' ' -f 4)

if [ ! -f $lastfile ]; then
	git log | head -n1 | cut -d ' ' -f 2 >$lastfile
fi


while [ 1 ]; do
	lastknown=$(cat $lastfile)
	while ! git pull; do
		sleep 60
	done

	git log | sed "/^commit ${lastknown}/q" | grep -v "^commit ${lastknown}" >$newfile || true
	if [ $(wc -l $newfile | cut -d ' ' -f 1) -gt 0 ]; then
		head -n1 $newfile | cut -d ' ' -f 2 >$lastfile

		cat $newfile | while read line; do
			if echo "$line" | grep '^commit ' >/dev/null; then
				if [ $(wc -c $ircfile | cut -d ' ' -f 1) -gt 0 ]; then
					echo '' >>$ircfile
					cat $ircfile >>$ircinp
				fi

				hash="$(echo "$line" | cut -d ' ' -f 2)"
				shash="$(echo "$hash" | grep -o '^......')..."
				link="http://github.com/${owner}/${repo}/commit/${hash}"
				short=$(curl -F "url=${link}" http://git.io/create)
				short="http://git.io/${short}"
				/usr/bin/printf "${repo}/${branch}: [${shash} \x036${short}\x0f]: ">$ircfile
			elif echo "$line" | grep '^Author: ' >/dev/null; then
				author="$(echo "$line" | cut -d '<' -f 1 | sed -e 's/^Author: //' -e 's/  *$//')"
				/usr/bin/printf "\x034%s\x0f:" "$author" >>$ircfile
			elif echo "$line" | grep '^Date: ' >/dev/null; then
				:;
			elif echo "$line" | grep '^$' >/dev/null; then
				:;
			elif echo "$line" | grep '^[ 	]*$' >/dev/null; then
				printf ' \n ' >>$ircfile
			else
				/usr/bin/printf " \`\`\x037%s\x0f''" "$line" >>$ircfile
			fi
		done

		if [ $(wc -c $ircfile | cut -d ' ' -f 1) -gt 0 ]; then
			echo '' >>$ircfile
			cat $ircfile >>$ircinp
			>$ircfile
		fi
	fi

	sleep 60
done
