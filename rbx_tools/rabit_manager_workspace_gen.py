import os
import shutil
import filecmp
import argparse
import sys

try:
    import em
except ImportError as e:
    print("Failed to import em: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user empy")
    print("")
    sys.exit(1)

try:
    import yaml
except ImportError as e:
    print("Failed to import yaml: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user pyyaml")
    print("")
    sys.exit(1)


try:
    from packaging import version
except ImportError as e:
    print("Failed to import packaging: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user packaging")
    print("")
    sys.exit(1)

def get_args():
    parser = argparse.ArgumentParser(
        description='Convert msg files to headers/sources')
    parser.add_argument('--header', help='Generate header files',
                        action='store_true')
    parser.add_argument('--source', help='Generate source files',
                        action='store_true')
    parser.add_argument('-i', dest='include_file',
                        help='header file')
    parser.add_argument('-c', dest='source_file',
                        help='source output file')                   
    parser.add_argument('-o', dest='outputdir',
                        help='output directory for workspace')
    parser.add_argument('-p', dest='template_h',
                        help='header template file')
    parser.add_argument('-q', dest='template_cpp',
                        help='header template file')
    parser.add_argument('-y', dest='msg_cfg',
                        help='header template file')
    parser.add_argument('-m', dest='messages',
                        help='messages used in the system',
                        nargs="+")
    parser.add_argument('-n', dest='name_space',
                        help='name space to put messages in')
                    
    return parser.parse_args()

class GenerateWorkspace(object):

    def __init__(self, args):
        self.template_h = args.template_h
        self.template_cpp = args.template_cpp
        self.msg_cfg_yml = args.msg_cfg
        self.outputdir = args.outputdir
        self.header = args.include_file
        self.source = args.source_file
        self.names_space = args.name_space
        self.is_header = args.header
        self.is_source = args.source
        self.msg_data = {}
        self.messages = args.messages

    @classmethod
    def generate(cls, args):
        g = cls(args)
        print("[rabit] Generating {}".format(os.path.splitext(g.header)[0]))
        g.import_yaml_message_config()
        g.generate_ws_header()
        g.generate_ws_source()

    def import_yaml_message_config(self):
        with open(self.msg_cfg_yml) as f:
            self.msg_data = yaml.load(f, Loader=yaml.FullLoader)

        if self.msg_data["topics"]:
            for topic in self.msg_data["topics"]:
                if topic["msg"] not in self.messages:
                    print("[rabit] :ERROR: These are the known system wide messages:")
                    for msg in self.messages:
                        print("[rabit]      - {}".format(msg))
                    raise ValueError("message[{}] not in list of system wide messages".format(topic["msg"]))
    
    def generate_ws_header(self):
        print("[rabit]        gen: {}".format(self.header))
        self._touch_temporary_output_director()
        output_file = os.path.join(self.outputdir, self.header)
        self._call_empy_and_create_generated_file(self.template_h, output_file)
    
    def generate_ws_source(self):
        print("[rabit]        gen: {}".format(self.source))
        self._touch_temporary_output_director()
        output_file = os.path.join(self.outputdir, self.source)
        self._call_empy_and_create_generated_file(self.template_cpp, output_file)

    def _touch_temporary_output_director(self):
         # Make sure output directory exists:
        if not os.path.isdir(self.outputdir):
            os.makedirs(self.outputdir)

    def _call_empy_and_create_generated_file(self, template_file, output_file):
        folder_name = os.path.dirname(output_file)
        if not os.path.exists(folder_name):
            os.makedirs(folder_name)

        em_globals = {
            "nm_space": self.names_space,
            "class_name": os.path.splitext(self.header)[0],
            "msg_data": self.msg_data
        }

        ofile = open(output_file, 'w')
        interpreter = em.Interpreter(output=ofile, globals=em_globals, options={
            em.RAW_OPT: True, em.BUFFERED_OPT: True})
        try:
            interpreter.file(open(template_file))
        except OSError as e:
            ofile.close()
            os.remove(output_file)
            raise
        interpreter.shutdown()
        ofile.close()
    

if __name__ == "__main__":
    
    args = get_args()
    GenerateWorkspace.generate(args)

