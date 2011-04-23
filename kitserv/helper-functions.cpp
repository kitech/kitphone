
#include "helper-functions.h"

Sid::String ask(Sid::String what)
{
        char* buffer = new char[KB_BUF_SIZE];
        memset(buffer, 0, KB_BUF_SIZE);
        
        printf("\n::: %s\n", (const char*)what);
        fflush(stdout);
        if (!fgets(buffer, KB_BUF_SIZE - 1, stdin)) { /*error*/ }

        if (buffer[0] == '\n') { delete[] buffer; return Sid::String(""); }
        if (char* ps = strchr(buffer, '\n')) *ps = '\0';
        Sid::String result = Sid::String(buffer);
        delete[] buffer;

        return result;
}

int ask_list(Sid::String what, const char* possibilities[])
{
        int num_choices = 0;
        char res[12];
        Sid::String choices = what + "\n";
        Sid::String num;
        while (Sid::String(possibilities[num_choices]) != "") {
                snprintf(res, 12, "%d", num_choices+1);                
                num = res; 
                choices += "	(" + num + ") " + possibilities[num_choices] + "\n";
                num_choices++;
        }
        choices += "? ";
        int resp = atoi((const char*)ask(choices));
        if (resp < 1 || resp > num_choices) return 0;
        return resp -1;
}

int ask_yesno(Sid::String what)
{
        Sid::String rsp = ask(what+" (Y/n)? ");
        static const char* no[] = { "N", "n", "No", "NO", "non", "NON", "ei", "Ei", "EI", "Nein", "nein", "NEIN", "" };
        for (int how_to_say_no = 0; Sid::String(no[how_to_say_no]) != ""; how_to_say_no++)
                if (rsp == no[how_to_say_no]) return 1;
        return 0;
}

int ask_int(Sid::String what)
{
        return atoi(ask(what));
}

bool load_file(Sid::String filename, Sid::Binary& buffer, const char* mode)
{
        FILE* f;
        if ((f = fopen((const char*)filename, mode)) != 0) {
                fseek(f, 0, SEEK_END);
                size_t fsize = ftell(f);
                rewind(f);
                buffer.resize(fsize);
                if (fsize) {
                        size_t read = fread(buffer.data(),1,fsize,f);
                        if (read != fsize) { /*error*/ }
                }
                fclose(f);
                return true;
        }
        return false;
}

