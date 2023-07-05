for ((i=10; i<=100; i+=10))
do
    for ((j=100; j<=10000; j+=100))
    do
        for ((p=1; p<=10; p+=1))
        do
            file_name=experiments/scip_$i\_$j\_$p.sh 
            echo "#!/bin/sh" > $file_name
            echo "./dp instances/$i\_$j\_$p.txt results/dp/$i\_$j\_$p.sol" >> $file_name
            sbatch --partition opt_int $file_name
            rm $file_name
        done 
    done 
done