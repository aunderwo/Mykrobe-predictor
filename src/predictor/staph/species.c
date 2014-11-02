/*
 * Copyright 2014 Zamin Iqbal (zam@well.ox.ac.uk)
 * 
 *
 * **********************************************************************
 *
 * This file is part of Mykrobe.
 *
 * Mykrobe is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mykrobe is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mykrobe.  If not, see <http://www.gnu.org/licenses/>.
 *
 * **********************************************************************
 */
/*
  species.c
*/


// system headers
#include <stdlib.h>
#include <limits.h>

// third party headers
#include <string_buffer.h>

#include "build.h" 
#include "maths.h" 
#include "element.h"
#include "seq.h"
#include "open_hash/hash_table.h"
#include "dB_graph.h"
#include "species.h"
#include "gene_presence.h"
#include "genotyping_known.h"





void get_coverage_on_best_catalayse_gene(dBGraph *db_graph,int max_branch_len,
                                        StrBuf* install_dir,int ignore_first, 
                                        int ignore_last,
                                        int* percentage_cov_cat,
                                        Covg* median_cov_cat)
{
  FILE* fp;
  AlleleInfo* ai = alloc_allele_info();
  //----------------------------------
  // allocate the memory used to read the sequences
  //----------------------------------
  Sequence * seq = malloc(sizeof(Sequence));
  if (seq == NULL){
    die("Out of memory trying to allocate Sequence");
  }
  alloc_sequence(seq,max_branch_len,LINE_MAX);

  //We are going to load all the bases into a single sliding window 
  KmerSlidingWindow* kmer_window = malloc(sizeof(KmerSlidingWindow));
  if (kmer_window==NULL)
    {
      die("Failed to malloc kmer sliding window");
    }


  CovgArray* working_ca = alloc_and_init_covg_array(max_branch_len);
  dBNode** array_nodes = (dBNode**) malloc(sizeof(dBNode*)*max_branch_len);
  Orientation* array_or =(Orientation*)  malloc(sizeof(Orientation)*max_branch_len);
  kmer_window->kmer = (BinaryKmer*) malloc(sizeof(BinaryKmer)*(max_branch_len-db_graph->kmer_size+1));
  if (kmer_window->kmer==NULL)
    {
      die("Failed to malloc kmer_window->kmer");
    }
  kmer_window->nkmers=0;
  //create file readers
  int file_reader_fasta(FILE * fp, Sequence * seq, int max_read_length, boolean new_entry, boolean * full_entry){
    long long ret;
    int offset = 0;
    if (new_entry == false){
      offset = db_graph->kmer_size;
    }
    ret =  read_sequence_from_fasta(fp,seq,max_read_length,new_entry,full_entry,offset);
    
    return ret;
  }
  
  
  
  if ( (array_nodes==NULL) || (array_or==NULL))
    {
      die("Cannot alloc array of nodes or of orientations");
    }

  // Does the data support the existance of catalayse?
  StrBuf* catalayse_fasta_file = strbuf_create(install_dir->buff);
  strbuf_append_str(catalayse_fasta_file, "data/staph/species/cat.fasta");
  fp = fopen(catalayse_fasta_file->buff, "r");
  if (fp==NULL)
    {
      die("Cannot open this file - %s", catalayse_fasta_file->buff);
    }
  // while the entry is valid iterate through the fasta file

  int num_kmers=0;
  int per_cov_cat=0;
  Covg med_cov_cat=0;
  do {
    num_kmers= get_next_single_allele_info(fp, db_graph, ai,
             true,
             seq, kmer_window,
             &file_reader_fasta,
             array_nodes, array_or, 
             working_ca, max_branch_len,
             ignore_first, ignore_last);
    if (ai->percent_nonzero >= per_cov_cat)
    {
      if (ai->median_covg_on_nonzero_nodes > med_cov_cat)
      {
        per_cov_cat=ai->percent_nonzero;;
        med_cov_cat=ai->median_covg_on_nonzero_nodes;
      }
    }
  } while ( num_kmers>0);
  *percentage_cov_cat = per_cov_cat;
  *median_cov_cat = med_cov_cat ;
  fclose(fp); 

}

boolean catalayse_exists_in_sample(dBGraph *db_graph,int max_branch_len,
                                StrBuf* install_dir,int ignore_first, 
                                int ignore_last)
{   
    int percentage_cov_cat=0;
    Covg median_cov_cat=0;
    get_coverage_on_best_catalayse_gene(db_graph,max_branch_len,
                                        install_dir,ignore_first, 
                                        ignore_last,
                                        &percentage_cov_cat,
                                        &median_cov_cat);
    if (percentage_cov_cat > 50)
    {
      return true;
    }   
    else
    {
      return false;
    }

}

boolean sample_is_staph(dBGraph *db_graph,int max_branch_len,
                    StrBuf* install_dir,int ignore_first, 
                    int ignore_last)
{
  // Check if catalayse exists
  if (catalayse_exists_in_sample(db_graph,max_branch_len,
                                install_dir,ignore_first, 
                                ignore_last))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// bool coverage_exists_on_multiple_panels()
// {
//   // get the coverage on all our panels
//   // do we have > 10% coverage on more than one panel
// }

// bool sample_is_mixed()
// {
//   if (coverage_exists_on_multiple_panels())
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

SampleType get_species_type(dBGraph *db_graph,int max_branch_len, 
                            StrBuf* install_dir,int expected_covg,
                            int ignore_first,int ignore_last)

{
  SampleType sample_type;
  if (sample_is_staph(db_graph,max_branch_len,
                      install_dir,ignore_first, 
                      ignore_last))
  {
    // if (sample_is_mixed())
    // {
    //   sample_type=MixedStaph;
    // }
    // else
    // {
      sample_type=PureStaphAureus;
    // }
  }
  else
  {
    sample_type = NonStaphylococcal;
  }
  return sample_type;
}



// Staph_species get_best_hit(int* arr_perc_cov, 
// 			   Covg* arr_median, 
// 			   boolean* found, 
// 			   boolean exclude_aureus)
// {
//   int i;
//   int prod=0;
//   int curr=-1;
//   for (i=0; i<NUM_SPECIES; i++)
//     {
//       if ( (exclude_aureus==true) && ((Staph_species)i==Aureus))
// 	{
// 	  continue;
// 	}
//       //      if (arr_perc_cov[i] * arr_median[i]>prod)
//       if (arr_perc_cov[i] > prod)
// 	{
// 	  //prod = arr_perc_cov[i]* arr_median[i];
// 	  prod =arr_perc_cov[i];
// 	  curr=i;
// 	}
//     }
//   if (curr==-1)
//     {
//       *found=false;
//       return Aureus;
//     }
//   else
//     {
//       *found=true;
//       return (Staph_species) curr;
//     }
// }

