for ((i=10; i<=100; i+=10))
do
    for ((j=100; j<=10000; j+=100))
    do
        for ((p=1; p<=10; p+=1))
        do
            file_name=experiments/$i\_$j\_$p.sh 
            echo "#!/bin/sh" > $file_name
            echo "python instance_generation_and_scip_answers.py $i $j 200 $p" >> $file_name
            sbatch --partition opt_int $file_name 
        done 
    done 
done