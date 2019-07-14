import sys

CFG = "defaults.cfg"
sep = ";"

def get_all():
    result = {}
    try:
        f = open(CFG, "r")
        for line in f:
            if not "=" in line:
                continue
            line = line.strip()
            #print(line)
            s = line.split("=",1)
            k,v = s[0],s[1]
            #print("read: ", k,v)
            result[k] = v
        f.close()
        return result
    except:
        print(("Could not load "+CFG, sys.exc_info()))
        return {}

def get(attribute):
    a = get_all()
    if attribute in list(a.keys()):
        r = a[attribute]
        r = r.replace(sep, "\n")
        return r
    else:
        return ""

def set(attribute, value):
    a = get_all()
    a[attribute] = value.replace("\n", sep)
    f = open(CFG, "w")
    for k,v in a.items():
        s = k+"="+v+"\n"
        #print ("write: ", s)
        f.write(s)
    f.close()

def set_multiline(attribute, value):
    value = sep.join(value)
    set(attribute, value)

def get_multiline(attribute):
    v = get(attribute)
    return v.split("\n")



