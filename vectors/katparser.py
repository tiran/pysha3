#!/usr/bin/env python3
import glob


def katparser(katfile):
    """Trivial parser for KAT files
    """
    length = msg = md = None
    with open(katfile) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            key, value = line.split(" = ")
            if key == "Len":
                length = int(value)
            elif key == "Msg":
                if not length:
                    msg = ''
                else:
                    msg = value
            elif key in ("MD", "Squeezed"):
                md = value
                if length % 8 == 0:
                    yield msg, md
                length = msg = md = None
            else:
                raise ValueError(key)


def main():
    for filename in sorted(glob.glob("ShortMsgKAT_*.txt")):
        exportname = filename[12:].lower().replace('-', '_')
        with open(exportname, 'w') as f:
            f.write("# {}\n".format(filename))
            for msg, md in katparser(filename):
                f.write("{},{}\n".format(msg.lower(), md.lower()))
            f.write("# EOF\n")


if __name__ == '__main__':
    main()
