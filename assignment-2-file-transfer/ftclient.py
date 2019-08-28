'''
Author: Derek Yang
Course: CS372 Intro to Networks
Description: 
The following client will communicate with a destination server
to send the commands -l or -g to either retrieve a list of the
server's directory (-l) or get a file from the server (-g)

Last Modified: 3/7/2019 at 11PM PT

'''


import socket
import sys
import time
import os


def sendCommand(sockfd):
    """
    Summary:
        Routes commands to the server.
    Description:
        Uses the data_port helper function to send and
        receive commands.  
    Parameters:
        sockfd (socket): uses sockfd to send and receive data
    """
    port = ""
    file_name = ""

    command_send = " ".join(sys.argv[3:])
    command_send = command_send +" "+ socket.gethostbyname(socket.gethostname())

    if sys.argv[3] == "-l":
        port = sys.argv[4]
    elif sys.argv[3] == "-g":
        file_name = sys.argv[4]
        port = sys.argv[5]
    
    data_port(port, sockfd, command_send)


def data_port(port, sockfd, full_command):
    """
    Summary:
        Binds the server at the ip and port'.
    Description:
        Helper function that helps to bind the server at the ip
        and port.  Will either print directory structure from server
        (-l command) or will save a file to the client from server
        (-g command)  
    Parameters:
        port : uses sockfd to send and receive data
        sockfd (socket): socket file description for connection
        full_command (string): unfiltered string from argv 
    """
    dataport = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    dpbind = dataport.bind(('', int(port)))
    dataport.listen(1)
    sockfd.send(full_command.encode())
    s_socket, address = dataport.accept()

    if sys.argv[3] == "-l":
        print("Receiving directory structure from {}:{}".format(sys.argv[1], sys.argv[2]))
        print(receive_data(s_socket))
    elif sys.argv[3] == "-g":
        getFileCommand(sys.argv[4], s_socket)
    s_socket.close()


def getFileCommand(file_name, sockfd):
    """
    Summary:
        Receives file from server and saves to local if found.
    Description:
        Helper function that helps to get a file from the server
        with the -g command.  If the file is not on the server
        a fail key is sent over to show that the file is not found.
        If the file is found, the function will call the
        create_file() helper function to save the file.
    Parameters:
        file_name (string): unfiltered string from argv 
        sockfd (socket): socket file description for connection
    """
    key = "6B1ACA4C28E2B1AE3F8C23D62D26B"
    data = receive_data(sockfd)

    print("Receiving \"{}\" from {}:{}".format(sys.argv[4],sys. argv[1], sys.argv[2]))
    if data == key:
        print("{}:{} says FILE NOT FOUND".format(sys.argv[1], sys.argv[2]))
    else:
        create_file(file_name, data)
        print("File transfer complete.")



def receive_data(sockfd):
    """
    Summary: 
        Receives incoming data and checks if all data is received.
    Description: 
        Loops until all data is received.  Removes the extra 
        terminal character sent by the server for a complete string.
    Parameters:
        sockfd (socket): socket file description for connection
    cited: 
        https://stackoverflow.com/questions/53285659/how-can-i-wait-until-i-receive-data-using-a-python-socket
    """
    data = ""
    while(True):
        receive = sockfd.recv(1024)
        if not receive:
            break
        data = data + receive.decode()
    # remove extra terminal char
    return(data[:-1])


def create_file(file_name, file_content):

    """
    Summary: 
        Creates a file and appends a number if there is a duplicate.
    Description: 
        Helper function that creates a file and labels the file with the
        prefix n_ if there is a duplicate found on the client end.
    Parameters:
        file_name (string): name of file
        file_content (string): content to be pasted into file
    cited: 
        cited: https://stackoverflow.com/questions/17984809/how-do-i-create-a-incrementing-filename-in-python
    """

    i = 0
    while os.path.exists(("{}_"+file_name).format(i)):
        i += 1

    f = open(("{}_"+file_name).format(i), "w")
    f.write(file_content)
    f.close()

def valid_args():
    """
    Summary: 
        Validates the arguments.
    Description: 
        Checks if the argv array is of correct length.  Also verifies
        -l and -g commands are sent with correct number of arguments.
    Parameters:
        None
    Return(bool):
        True if correct args
        False if incorrect args
    cited: 
    """
    if len(sys.argv) < 4:
        return False
    elif ((sys.argv[3] == "-l") and (len(sys.argv) == 5))\
            or ((sys.argv[3] == "-g") and ( len(sys.argv) == 6)):
        return True
    else:
        return False



if __name__ == "__main__":

    print("\n==ftclient==")
    # validate command
    if not valid_args():
        print("Error, please use -l to list files or -g to grab file\n"
              "Format:\n"
              "<serverhostname> <serverport> <-l> <dataport>\n"
              "<serverhostname> <serverport> <-g> <filename> <dataport>")
        exit(1)
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((sys.argv[1], int(sys.argv[2])))
    sendCommand(client)
    print("--Closed Connection--")