import os
import tarfile
from urllib import request

SMALL_SEQUENCE_LEN = 10
MEDIUM_SEQUENCE_LEN = 20
LARGE_SEQUENCE_LEN = 40
EXTRA_LARGE_SEQUENCE_LEN = 100

SMALL_TESTS_FREQ = 0.4
MEDIUM_TESTS_FREQ = 0.3
LARGE_TEST_FREQ = 0.2
EXTRA_LARGE_TEST_FREQ = 0.1

MAX_SEQUENCES_NUM = 5


class AlignmentData:
    def __init__(self, file_prefix, sequences, alignment):
        self.file_prefix = file_prefix
        self.sequences = sequences
        self.alignment = alignment


def resize_test(alignment_data, size):
    if len(alignment_data.alignment[0]) <= size:
        return alignment_data
    b = -1
    for j in range(len(alignment_data.alignment[0]) - size + 1):
        sol = True
        for a in alignment_data.alignment:
            if all(c == '-' for c in a[j: size + j]):
                sol = False
        if sol:
            b = j
            break
    assert b != -1
    alignment_data.alignment = [a[b:size + b] for a in alignment_data.alignment]
    alignment_data.sequences = [''.join([c for c in a if c != '-']) for a in alignment_data.alignment]
    return alignment_data


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


def download_data():
    if os.path.exists('bb3_release'):
        print(' Already downloaded.')
        return
    datastream = request.urlopen('http://www.lbgi.fr/balibase/BalibaseDownload/BAliBASE_R1-5.tar.gz')
    thetarfile = tarfile.open(fileobj=datastream, mode="r|gz")
    thetarfile.extractall()


def process_data(output_dir='data/sequences', balibase_dir='bb3_release'):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    data = []
    for root, dirs, files in os.walk(balibase_dir):
        if len(files) == 0:
            continue
        files = list(filter(lambda s: s.startswith('BB'), files))
        file_prefixes = list(set([f.split('.')[0] for f in files]))
        file_prefixes.sort()
        for file_prefix in file_prefixes:
            alignment_data = parse_file(root, file_prefix)
            if len(alignment_data.sequences) <= MAX_SEQUENCES_NUM:
                data.append(alignment_data)

    for i in range(len(data)):
        if i < SMALL_TESTS_FREQ * len(data):
            data[i] = resize_test(data[i], SMALL_SEQUENCE_LEN)
        elif i < (SMALL_TESTS_FREQ + MEDIUM_TESTS_FREQ) * len(data):
            data[i] = resize_test(data[i], MEDIUM_SEQUENCE_LEN)
        elif i < (SMALL_TESTS_FREQ + MEDIUM_TESTS_FREQ + LARGE_TEST_FREQ) * len(data):
            data[i] = resize_test(data[i], LARGE_SEQUENCE_LEN)
        else:
            data[i] = resize_test(data[i], EXTRA_LARGE_SEQUENCE_LEN)
        save_to_file(data[i], os.path.join(output_dir, data[i].file_prefix + '.txt'))

    with open('data/sequences/all_files.txt', 'w') as f:
        print(' '.join([d.file_prefix + '.txt' for d in data]), file=f)


def construct_dataset():
    print('Downloading BALiBASE dataset...')
    download_data()
    print('Processing dataset...')
    process_data()


if __name__ == '__main__':
    construct_dataset()
