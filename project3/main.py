#Aditi Nataraj
#Amy Zheng
import sys

size = 3

t_memmove = 1
class Procedure(object):
    def __init__(self, name, pages, everythingelse):
        self.name = name
        self.num_pages = pages
        self.curr_arrival = everythingelse[0].split('-')[0]
        self.curr_departure = everythingelse[0].split('-')[1]
        self.other = everythingelse

    
def move_shit(totalist, temp_time, time):
    for i in totalist:
        if int(i.curr_arrival) >= time:
            i.curr_arrival = str(int(i.curr_arrival) + temp_time)
        if int(i.curr_departure) >= time:
            i.curr_departure = str(int (i.curr_departure) + temp_time)
        newlist = []
        for y in i.other:
            x = y.split('-')
            amy = ""
            if int(x[0]) >= time:
                amy += str(int(x[0]) + temp_time)
            else:
                amy += x[0]
            amy+="-"
            if int(x[1]) >= time:
                amy += str(int(x[1]) + temp_time)
            else:
                amy += x[1]
            newlist.append(amy)
        i.other = newlist
    return totalist
            
                
def print_board(board):
    x = ""
    print "=" * 32
    j = 1
    for i in range(0, 256):
        x += board[i]
        if (j % 32 == 0 or j == 256):
           print x
           x = ""
        j+=1
    print "=" * 32
    
def find_blank_space(board):
    count = 0
    for i in range(256):
        if board[i] == '.':
            count+=1
    return count

def consec_dots(board, i):
    board_index = board.index('.')
    c = 0
    temp = board_index
    while (temp < 256):
        if board[temp] == '.' and c < int(i.num_pages):
            c += 1
        elif board[temp] != '.' and c < int(i.num_pages):
            board_index = temp
            board_index += 1
            c = 0
        if c == int(i.num_pages):
            break
        temp += 1
    return c, board_index
    
def consec_dots_nf(board, i, starting, phresh):
    board_index = board.index('.')
    c = 0
    #print phresh
    #print starting
    if len(phresh) > 0:
        temp = phresh[-1]
    else:
        temp = starting
    #print temp
    #print temp+starting
    temp1 = temp+starting
    found = False
    #print "c is " + str(c)
    #print "board index is " + str(board_index)
    board_index = temp1
    while (temp1 < 256):
        if board[temp1] == '.' and c < int(i.num_pages):
            c += 1
        elif board[temp1] != '.' and c < int(i.num_pages):
            board_index = temp1
            board_index += 1
            c = 0
        if c == int(i.num_pages):
            break
        temp1 += 1
    #print "c is " + str(c)
    #print "board index is " + str(board_index)
    if (c < int(i.num_pages)):
        c = 0
        board_index = 0
        while (temp < 256):
            if board[temp] == '.' and c < int(i.num_pages):
                c += 1
            elif board[temp] != '.' and c < int(i.num_pages):
                board_index = temp
                board_index += 1
                c = 0
            if c == int(i.num_pages):
                break
            temp += 1
    return c, board_index
    

def consec_dots_nf2(board, i, starting, phresh):
    board_index = board.index('.')
    c = 0
    #print phresh
    #print starting
    if len(phresh) > 0:
        temp = phresh[-1]
    else:
        temp = starting
    #print temp
    #print temp+starting
    #print "c is " + str(c)
    #print "board index is " + str(board_index)
    while (temp < 256):
        if board[temp] == '.' and c < int(i.num_pages):
            c += 1
        elif board[temp] != '.' and c < int(i.num_pages):
            board_index = temp
            board_index += 1
            c = 0
        if c == int(i.num_pages):
            break
        temp += 1
    #print "c is " + str(c)
    #print "board index is " + str(board_index)
    return c, board_index



def consec_dots_bf(board, i):
    board_index = board.index('.')
    c = 0 
    temp = board_index
    all_consec = {}
    while(temp < 256):
        if board[temp] == '.' :
            c += 1
        elif board[temp] != '.':
            board_index = temp
            board_index += 1
            all_consec.update({board_index: c})
            c = 0
        all_consec.update({board_index: c})
        temp += 1
    for z in all_consec.keys():
        if all_consec[z] < int(i.num_pages):
            del all_consec[z]
    tmp = sorted(all_consec.items(), key = lambda x:(x[1], x[0]))
    return tmp[0][1], tmp[0][0]
    
def contiguous(board, totalist, is_firstfit, is_nextfit, is_bestfit):
    time = 0
    def_time = 0
    print_b = True
    def_frame = 0
    defcon = False
    temp_time = 0
    found = False
    starting = 0
    freshest = []
    if is_firstfit:
        print "time " + str(time) + "ms: Simulator started (Contiguous -- First-Fit)"
    elif is_nextfit:
        print "time " + str(time) + "ms: Simulator started (Contiguous -- Next-Fit)"
    elif is_bestfit:
        print "time " + str(time) + "ms: Simulator started (Contiguous -- Best-Fit)"
    
    while (True):
        found = False
        for y in totalist:
            if len(y.other) != 0:
                found = True
        if not found:
            time -= 1
            if is_firstfit:
                print "time " + str(time) + "ms: Simulator ended (Contiguous -- First-Fit)"
            elif is_nextfit:
                print "time " + str(time) + "ms: Simulator ended (Contiguous -- Next-Fit)"
            elif is_bestfit:
                print "time " + str(time) + "ms: Simulator ended (Contiguous -- Best-Fit)"
            break
        for i in totalist:
            if int(i.curr_arrival) == time:
                print "time " + str(time) + "ms: Process " + i.name + " arrived (requires " + i.num_pages + " frames of physical memory)"
                if int(i.num_pages) > int(find_blank_space(board)):
                    print "time " + str(time) +"ms: Cannot place process " + i.name + " -- skipping process " + i.name
                elif int(i.num_pages) < find_blank_space(board):
                    temp_time = time
                    other = 0
                    if is_firstfit:
                        c = (consec_dots(board, i))[0]
                        board_index = (consec_dots(board, i))[1]
                    elif is_bestfit:
                        c = (consec_dots_bf(board, i))[0]
                        board_index = (consec_dots_bf(board, i))[1]
                    elif is_nextfit:
                        c = (consec_dots_nf(board, i, starting, freshest))[0]
                        board_index = (consec_dots_nf(board, i, starting, freshest))[1]
                        if c+board_index >= 256:
                            c = (consec_dots(board, i))[0]
                            board_index = (consec_dots(board, i))[1]
                        starting = board_index
                    starting = board_index
                    if c >= int(i.num_pages):
                        while (other < int(i.num_pages)):
                            board[other+board_index] = i.name
                            other += 1
                        freshest.append(other)
                        print "time " + str(time) + "ms: Placed process " + i.name + " in memory:"
                    else:
                        print "time " + str(time) + "ms: Cannot place process " + i.name +" -- starting defragmentation"
                        defcon = True
                        print_b = False
                        spaces = find_blank_space(board)
                        if board[0] != '.':
                            first = board.count(board[0])
                            def_frame = 256 - first - spaces
                            def_time =  def_frame + time
                        else:
                            def_frame = 256 - spaces
                            def_time = def_frame + time
                        temp_time = def_time - time
                        c = move_shit(totalist, temp_time, time)
                        time = def_time
                        
                        temp_processes = []
                        if (board[0] != '.'):
                            temp = board[0]
                            for j in range(256):
                                if board[j] != temp and board[j] != '.':
                                    if board[j] not in temp_processes:
                                        temp_processes.append(board[j])
                            if time == def_time and def_time != 0:
                                if is_nextfit:
                                    freshest.append(other)
                                if '.' in board:
                                    board.remove('.')
                                board = [x for x in board if x != '.']
                            for j in range (spaces):
                                board.append('.')
                            if len(temp_processes) != 0:
                                print "time " + str(def_time) + "ms: Defragmentation complete (moved " + str(def_frame) + " frames:",
                                for x in range(len(temp_processes)):
                                    if ( x!= len(temp_processes)- 1):
                                        print temp_processes[x] +",",
                                    else:
                                        print temp_processes[x] + ")"
                            print_board(board) 
                            print "time " + str(time) + "ms: Placed process " + i.name + " in memory:"
                            ind = board.index('.')
                            for j in range(ind, ind+int(i.num_pages)):
                                board[j] = i.name
                            print_board(board)
                            
                      
                if print_b:
                    print_board(board)
                else:
                    print_b = True
                     
            elif int(i.curr_departure) == time:
                if i.name in board:
                    ind =  board.index(i.name)
                    print "time " + str(time) + "ms: Process " + i.name + " removed from physical memory"
                    for j in range(ind, ind + int(i.num_pages)):
                        board[j] = '.'
                    print_board(board)
                i.other.remove((i.other)[0])
                if not len(i.other) ==  0:
                    i.curr_arrival = (i.other)[0].split('-')[0]
                    i.curr_departure = (i.other)[0].split('-')[1]
        time += 1


def noncontiguous(board, totalist, is_firstfit, is_nextfit, is_bestfit):
    time = 0
    def_time = 0
    def_frame = 0
    temp_time = 0
    found = False
    starting = 0
    print "time " + str(time) + "ms: Simulator started (Non-contiguous)"
    while (True):
        found = False
        for y in totalist:
            if len(y.other) != 0:
                found = True
        if not found:
            time -= 1
            print "time " + str(time) + "ms: Simulator ended (Non-contiguous)"
            break
        for i in totalist:
            if int(i.curr_arrival) == time:
                print "time " + str(time) + "ms: Process " + i.name + " arrived (requires " + i.num_pages + " frames of physical memory)"
                if int(i.num_pages) > int(find_blank_space(board)):
                    print "time " + str(time) +"ms: Cannot place process " + i.name + " -- skipping process " + i.name
                    print_board(board)
                elif int(i.num_pages) < find_blank_space(board):
                    other = 0
                    count = 0
                    j = 0
                    while j < 256 and count < int(i.num_pages):
                        if board[j] == '.':
                            board[j] = i.name
                            count += 1
                        if count == int(i.num_pages):
                            break
                        j += 1
                    print "time " + str(time) + "ms: Placed process " + i.name + " in memory:"
                    print_board(board)

            elif int(i.curr_departure) == time:
                if i.name in board:
                    ind =  board.index(i.name)
                    print "time " + str(time) + "ms: Process " + i.name + " removed from physical memory"
                    for j in range(ind, 256):
                        if board[j] == i.name:
                            board[j] = '.'
                    print_board(board)
                i.other.remove((i.other)[0])
                if not len(i.other) ==  0:
                    i.curr_arrival = (i.other)[0].split('-')[0]
                    i.curr_departure = (i.other)[0].split('-')[1]
        time += 1


def print_queue(da_queue, size):
    perf = "[mem:"
    for i in range(size):
        perf += " "
        perf += da_queue[i]
    perf += "]"
    return perf

def print_keep(keep_track):
    perf = ""
    for key in keep_track:
        temp = key + ":" + str(keep_track[key]) + ","
        perf+=temp
    return perf

def lfu(numbers, size):
    page_faults = 0
    #da_queue = ['.','.','.']
    da_queue=[]
    for i in range(size):
        da_queue.append('.')
    
    count = 0
    #f = open('out.txt', 'w+')
    print "Simulating LFU with fixed frame size of " + str(size)
    keep_track = {}
    for n in numbers:
        keep_track[n] = 0
    for n in numbers:
        if n in da_queue:
            print "referencing page " + n + " " + print_queue(da_queue, size)
            keep_track[n] += 1
            count+=1
            continue
        if '.' in da_queue:
            ind = da_queue.index('.')
            da_queue[ind] = n
            page_faults += 1
            print "referencing page " + n + " " + print_queue(da_queue, size) + \
                " PAGE FAULT (no victim page)"
            keep_track[n] += 1
        else:
            temp = []
            for i in range(size):
                #if da_queue[i] in numbers[count:]:
                i1 = keep_track[da_queue[i]]
                #else:
                 #   i1 = 65535
                temp.append(i1)
            correct_index = min(temp)
            which = -1
            for i in range(size):
                if correct_index == temp[i]:
                    which = i
            if (temp.count(correct_index) > 1):
                min_dex = -1
                for j in range(size):
                    if temp[j] == correct_index:
                        which_1 = str(min(int(da_queue[which]), int(da_queue[j])))
                        which = da_queue.index(which_1)
            this_is_the_one = da_queue[which]
            da_queue[which] = n
            page_faults += 1
            keep_track[n] = 1
            #keep_track[n] += 1
            #f.write(" " + print_keep(keep_track))
            print "referencing page " + n + " " + \
                print_queue(da_queue, size) + " PAGE FAULT (victim page " \
                + this_is_the_one + ")"
    print "End of LFU simulation (" + str(page_faults) + " page faults)"

def lru(numbers, size):
    page_faults = 0
    #da_queue = ['.','.','.']
    da_queue=[]
    for i in range(size):
        da_queue.append('.')
    
    oldest = 0
    #f = open('out.txt', 'w+')
    print "Simulating LRU with fixed frame size of " + str(size)
    #print da_queue
    count = 0
    which = -1
    keep_track = {}
    for n in numbers:
        keep_track[n] = 0
    for n in numbers:
        if n in da_queue:
            keep_track[n] = count
            print "referencing page " + n + " " + print_queue(da_queue, size) 
            count+=1
            continue
        if '.' in da_queue:
            ind = da_queue.index('.')
            da_queue[ind] = n
            page_faults += 1
            keep_track[n] = count
            print "referencing page " + n + " " + print_queue(da_queue, size) + \
                " PAGE FAULT (no victim page)"
        else:
            temp = []
            for i in range(size):
                #if da_queue[i] in numbers[count:]:
                i1 = keep_track[da_queue[i]]
                temp.append(i1)
            #print temp
            correct_index = min(temp)
            which = -1
            for i in range(size):
                if correct_index == temp[i]:
                    which = i
            if (temp.count(correct_index) > 1):
                #print "here?", da_queue[which]
                min_dex = -1
                for j in range(size):
                    if temp[j] == correct_index and j != which:
                        which_1 = str(min(int(da_queue[which]), int(da_queue[j])))
                        which = da_queue.index(which_1)
            
            this_is_the_one = da_queue[which]
            da_queue[which] = n
            page_faults += 1
            keep_track[n] = count
            #f.write(" " + print_keep(keep_track))
            print "referencing page " + n + " " + \
                print_queue(da_queue, size) + " PAGE FAULT (victim page " \
                + this_is_the_one + ")"
        count+=1
    print "End of LRU simulation (" + str(page_faults) + " page faults)"
            

def opt(numbers, size):
    page_faults = 0
    da_queue = []
    for i in range(size):
        da_queue.append('.')
    #f = open('out.txt', 'w+')
    #f.write("Simulating OPT with fixed frame size of 3\n")
    print "Simulating OPT with fixed frame size of " + str(size)
    #f.write(da_queue)
    count = 0
    which = -1
    for n in numbers:
        if n in da_queue:
            #f.write("referencing page " + n + " " + print_queue(da_queue) + "\n")
            print "referencing page " + n + " " + print_queue(da_queue, size)
            count+=1
            continue
        if '.' in da_queue:
            ind = da_queue.index('.')
            da_queue[ind] = n
            page_faults += 1
            print "referencing page " + n + " " + print_queue(da_queue, size) + \
                " PAGE FAULT (no victim page)"
            #f.write("referencing page " + n + " " + print_queue(da_queue) + \
            #    " PAGE FAULT (no victim page)\n")
        else:
            temp = []
            for i in range(size):
                if da_queue[i] in numbers[count:]:
                    i1 = numbers[count:].index(da_queue[i])
                else:
                    i1 = 65535
                temp.append(i1)
            correct_index = max(temp)
            which = -1
            for i in range(size):
                if correct_index == temp[i]:
                    which = i
            if (temp.count(correct_index) > 1):
                min_dex = -1
                for j in range(size):
                    if temp[j] == correct_index:
                        which_1 = str(min(int(da_queue[which]), int(da_queue[j])))
                        which = da_queue.index(which_1)
            this_is_the_one = da_queue[which]
            da_queue[which] = n
            page_faults += 1
            #f.write("index 1 is " + str(i1) + " index 2 is " + str(i2) + " index 3 is " + str(i3) + "\n")
            print "referencing page " + n + " " + \
                print_queue(da_queue, size) + " PAGE FAULT (victim page " \
                + this_is_the_one + ")"
            #f.write("referencing page " + n + " " + \
                #print_queue(da_queue) + " PAGE FAULT (victim page " \
                #+ this_is_the_one + ")\n")
        count+=1
    print "End of OPT simulation (" + str(page_faults) + " page faults)"
    #f.write("End of OPT simulation (" + str(page_faults) + " page faults)\n")
    


if __name__ == "__main__":
    f = open(sys.argv[1])
    f1 = open(sys.argv[2])
    lines = (f.read()).split('\n')
    i = 1
    totalist = []
    totalist2 = []
    totalist3 = []
    totalist4 = []
    totalist5 = []
    totalist6 = []
    
    while (i < len(lines)):
        if i > int(lines[0]):
            break
        x = lines[i].split(' ')
        if len(x) < 3:
            continue
        y = Procedure(x[0], x[1], x[2:])
        y2 = Procedure(x[0], x[1], x[2:])
        y3 = Procedure(x[0], x[1], x[2:])
        y4 = Procedure(x[0], x[1], x[2:])
        y5 = Procedure(x[0], x[1], x[2:])
        y6 = Procedure(x[0], x[1], x[2:])
        totalist.append(y)
        totalist2.append(y2)
        totalist3.append(y3)
        totalist4.append(y4)
        totalist5.append(y5)
        totalist6.append(y6)
        i+=1
    board = []
    board2 = []
    board3 = []
    board4 = []
    board5 = []
    board6 = []
    for i in range (256):
        board.append('.')
        board2.append('.')
        board3.append('.')
        board4.append('.')
        board5.append('.')
        board6.append('.')
    totcount = 0
    '''for i in range(256):
        print board[i],
        totcount += 1
        if (totcount % 32 == 0):
          print "\n"'''
    contiguous(board, totalist, True, False, False)
    print
    contiguous(board2, totalist2, False, True, False)
    print
    contiguous(board3, totalist3, False, False, True)
    print
    noncontiguous(board4, totalist4, True, False, False)
    print
    #noncontiguous(board3, totalist5, False, True, False)
    #noncontiguous(board6, totalist6, False, False, True)
    
    #f = open(sys.argv[1])
    e = f1.read().split('\n')
    #size = int(e[0])
    all_inputs = e[0].split(' ')
    #all_inputs[-1] = all_inputs[-1].rstrip()
    #print all_inputs
    
    opt(all_inputs, size)
    print
    lru(all_inputs, size)
    print
    lfu(all_inputs, size)
