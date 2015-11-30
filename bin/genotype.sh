# echo "Colour Covgs" $1 $2
comid=$1
data=$2
#cp panel_tb_k31.fasta /tmp/"$1"_"$4"_"$5".fasta
#readlink -f $2 > /tmp/"$1"_"$4"_"$5".blist
#ls /tmp/"$1"_"$4"_"$5".blist > /tmp/"$1"_"$4"_"$5".clist
#ls  /tmp/"$1"_"$4"_"$5".fasta > /tmp/"$1"_"$4"_"$5".panel
#/home/phelimb/git/cortex/bin/cortex_var_31_c1 --colour_list /tmp/"$1"_"$4"_"$5".clist --kmer_size $5 --mem_height 20 --mem_width 250 --align /tmp/"$1"_"$4"_"$5".panel,no --align_input_format LIST_OF_FASTA --max_read_len 10000

# rm -f /home/phelimb/git/atlas-core/panel.ctx
# rm -f /tmp/"$comid"_tb_31.ctx
# time /data2/apps/mccortex/bin/mccortex31 build -k 31 -q -s atlas -1 /home/phelimb/git/atlas-core/panel_tb_k31.fasta  /home/phelimb/git/atlas-core/panel.ctx
time /home/phelimb/git/mccortex/bin/mccortex31 build -q -k 31 -s $comid -1 $data /tmp/"$comid"_tb_31.ctx
time /data2/apps/mccortex/bin/mccortex31 coverage -q -s /home/phelimb/git/atlas-core/panel_tb_k31.fasta /tmp/"$comid"_tb_31.ctx > /tmp/"$comid"_tb_31.fasta.colour_covgs

# /data2/users/phelim/tools/jellyfish-2.2.0/bin/jellyfish count -o /tmp/"$1"_"$4"_"$5".jf -m $5 -c 3 -t 2 -s 10000000 <(zcat $2)
# echo "Comparing to PANEL" $1
# /data2/users/phelim/tools/jellyfish-2.2.0/bin/jellyfish query /tmp/"$1"_"$4"_"$5".jf -s panel_tb_k31.kmers > /tmp/"$1"_"$4"_"$5".count
# /home/phelimb/git/atlas-core/bin/genotype.py $1 $4 $5 /tmp/"$1"_"$4"_"$5".count --all #&& rm /tmp/"$1"_"$4"_"$5".jf && rm /tmp/"$1"_"$4"_"$5".count 