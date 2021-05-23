import argparse
import os


class AlignmentData:
    def __init__(self, file_prefix, sequences, alignment):
        self.file_prefix = file_prefix
        self.sequences = sequences
        self.alignment = alignment


def parse_sequences(file_name):
    with open(file_name) as f:
        sequences = []
        for l in f.readlines():
            if l == '\n':
                continue
            if l.startswith('>'):
                sequences.append("")
            else:
                sequences[-1] += l.rstrip()
    return sequences


def parse_alignment(file_name):
    with open(file_name) as f:
        lines = ''.join(f.readlines())
        alignment = [''.join(s.split('</seq-data>')[0].split('\n')).upper() for s in lines.split('<seq-data>')[1:]]
    return alignment


def parse_file(root_dir, file_prefix) -> AlignmentData:
    sequences = parse_sequences(os.path.join(root_dir, file_prefix) + '.tfa')
    alignment = parse_alignment(os.path.join(root_dir, file_prefix) + '.xml')
    return AlignmentData(file_prefix, sequences, alignment)


def save_to_file(alignment_data, save_file):
    with open(save_file, 'w') as f:
        for seq in alignment_data.sequences:
            print(seq, file=f)
        for aligned_seq in alignment_data.alignment:
            print(aligned_seq, file=f)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Process data from Balibase3 for heuristic multiple alignment.')
    parser.add_argument('--balibase_dir',
                        help='Directory with Balibase3 data. Can be downloaded here:'
                             'https://www.lbgi.fr/balibase/BalibaseDownload/BAliBASE_R1-5.tar.gz',
                        default='bb3_release')
    parser.add_argument('--max_sequences_num',
                        help='Maximum number of sequences to align.',
                        default=15)
    parser.add_argument('--output_dir',
                        help='Directory to score processed data.',
                        default='data/sequences')
    args = parser.parse_args()

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)
    all_files = []
    for root, dirs, files in os.walk(args.balibase_dir):
        if len(files) == 0:
            continue
        files = list(filter(lambda s: s.startswith('BB'), files))
        file_prefixes = list(set([f.split('.')[0] for f in files]))
        file_prefixes.sort()
        for file_prefix in file_prefixes:
            alignment_data = parse_file(root, file_prefix)
            sequences_num = len(alignment_data.sequences)
            if sequences_num > args.max_sequences_num:
                continue
            save_to_file(alignment_data, os.path.join(args.output_dir, file_prefix + '.txt'))
            all_files.append(file_prefix + '.txt')
    with open('data/sequences/all_files.txt', 'w') as f:
        print(' '.join(all_files), file=f)

