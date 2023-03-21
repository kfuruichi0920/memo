#!/bin/bash

t_sleep=2

get_address_list() {

        echo "Read File[$1]"

        while read rec_list
        do
                rec_head=`echo ${rec_list} | cut -c 1`

                # skip blank line
                if [ -z "${rec_head}" ]
                then
                        #echo "empty"
                        continue
                fi

                # skip comment line
                if [ "${rec_head}" = "#" ]
                then
                        #echo "skip"
                        continue
                fi

                # packet filter, routing
                col1=`echo ${rec_list} | cut -d , -f 1`
                col2=`echo ${rec_list} | cut -d , -f 2`
                col3=`echo ${rec_list} | cut -d , -f 3`

                echo [${col1}][${col2}][${col3}]


                sleep ${t_sleep}
                t_sleep=$((t_sleep*2))
                if [ ${t_sleep} -ge 8 ]
                then
                        t_sleep=8
                fi

        done <  $1
}


get_address_list $1
~
~
~
~
~
