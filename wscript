
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import os.path
import json

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')
    ctx.set_group('bundle')
	
	# Generate appinfo.h
    ctx(rule=generate_appinfo_h, source='appinfo.json', target='../src/generated/appinfo.h')
	
	# Generate appinfo.js
    ctx(rule=generate_appinfo_js, source='appinfo.json', target='../src/js/generated/appinfo.js')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        app_elf='{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
        target=app_elf)

        if build_worker:
            worker_elf='{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'),
            target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})
			
	# Concatenate all JS files into pebble-js-app.js prior to building.
	all_js = "\n".join([node.read() for node in ctx.path.ant_glob('src/js/**/*.js', excl='src/js/pebble-js-app.js')])
	out_js_node = ctx.path.make_node('src/js/pebble-js-app.js')
	out_js_node.write(all_js)

    ctx.pbl_bundle(binaries=binaries, js=out_js_node)

def generate_appinfo_h(task):
    src = task.inputs[0].abspath()
    target = task.outputs[0].abspath()
    appinfo = json.load(open(src))
    f = open(target, 'w')
    f.write('#pragma once\n\n')
    f.write('#define VERSION_LABEL "{0}"\n'.format(appinfo['versionLabel']))
    f.write('#define UUID "{0}"\n'.format(appinfo['uuid']))
    for key in appinfo['appKeys']:
    	f.write('#define APP_KEY_{0} {1}\n'.format(key.upper(), appinfo['appKeys'][key]))
    f.close()
	
def generate_appinfo_js(task):
    src = task.inputs[0].abspath()
    target = task.outputs[0].abspath()
    data = open(src).read().strip()
    f = open(target, 'w')
    f.write('var AppInfo = ')
    f.write(data)
    f.write(';\n\n')
    f.close()
