#!/usr/bin/env python3

import subprocess
import sys
import time

filestodelete = []

def atexit():
    if len(filestodelete) > 0:
        args = [ "rm", "-f" ]
        args.extend(filestodelete);
        subprocess.call(args)

def endtests(mess, mark, maxmark):
    print("End of tests: " + mess)
    print("Mark: %d/%d" % (mark, maxmark))
    atexit()
    sys.exit(0)

def c_compile(name, files, mark):
    args = ["gcc", "-o", name]
    args.extend(files)
    print(" ".join(args))
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()
    rcode = process.wait()
    if rcode != 0:
        print(err.decode('ascii'))
        endtests("Cannot compile", mark, 20)

def make(folder, mark, arg=None):
    args = ["make", "-C", folder ]
    if arg is not None:
        args.append(arg)
    print(" ".join(args))
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()
    rcode = process.wait()
    if rcode != 0:
        print(err.decode('ascii'))
        endtests("Cannot compile", mark, 20)

def get_test_output(mouliname):
    process = subprocess.Popen(mouliname, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()
    rcode = process.wait()
    return out.decode('ascii')

def get_user_output(username, marksofar):
    process = subprocess.Popen(username, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(1)
    out, err = process.communicate()
    rcode = process.wait()
    if rcode != 0:
        print(err.decode('ascii'))
        endtests("User exe returned non-zero", marksofar, 20)
    return out.decode('ascii')

def run_test(mainfile, moulifile, userfile, testname, testno, marksofar):
    mouliname = "./mouli%d" % testno;
    username = "./user%d" % testno;
    print("Test %d: %s" % (testno, testname))
    c_compile(mouliname, [mainfile, moulifile], marksofar)
    filestodelete.append(mouliname)
    outmouli = get_test_output(mouliname)
    c_compile(username, [ mainfile, userfile], marksofar)
    filestodelete.append(username)
    outuser = get_user_output(username, marksofar)
    if outmouli != outuser:
        print("Test failed")
        print("User output:")
        print(outuser)
        print("Expected output:")
        print(outmouli)
        endtests("Output differs", marksofar, 20)
    print("Test passed")

def main():
    totaltest = 1
    current_test = 0
    maxmark = 20
    print("Beginning of tests")
    print()
    run_test("main_01.c", "my_strcpy.c", "../ex_01/my_strcpy.c", "my_strcpy", 1, int(current_test / totaltest * maxmark)) ; current_test += 1 ; print()
    print("End of tests")
    print("Mark: %d/%d" % (int(current_test / totaltest * maxmark), maxmark))
    atexit()

if __name__ == "__main__":
    main()
    
