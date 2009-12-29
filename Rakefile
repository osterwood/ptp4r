require 'rubygems'
require 'rake'

begin
  require 'rake/extensiontask'
rescue LoadError
  puts "WARNING: rake-compiler is not installed. You will not be able to build the json gem until you install it."
end

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "ptp4r"
    gem.summary = %Q{Ruby library of libptp}
    gem.description = %Q{Ruby library exposing libptp (picture taking protocol)}
    gem.email = "catwood@gmail.com"
    gem.homepage = "http://github.com/catwood/ptp4r"
    gem.authors = ["Chris Atwood"]
    gem.add_development_dependency "thoughtbot-shoulda", ">= 0"
    gem.require_paths = ["ext"]
    gem.extensions = ["ext/ptp4r_ext/extconf.rb"]
    gem.files = FileList["[A-Z]*", "{ext,lib,test}/**/*", 'lib/jeweler/templates/.gitignore']
    # gem is a Gem::Specification... see http://www.rubygems.org/read/chapter/20 for additional settings
  end
  Jeweler::GemcutterTasks.new
rescue LoadError
  puts "Jeweler (or a dependency) not available. Install it with: gem install jeweler"
end

require 'rake/testtask'
Rake::TestTask.new(:test) do |test|
  test.libs << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

begin
  require 'rcov/rcovtask'
  Rcov::RcovTask.new do |test|
    test.libs << 'test'
    test.pattern = 'test/**/test_*.rb'
    test.verbose = true
  end
rescue LoadError
  task :rcov do
    abort "RCov is not available. In order to run rcov, you must: sudo gem install spicycode-rcov"
  end
end

Rake::ExtensionTask.new('ptp4r_ext')

task :test => :check_dependencies

task :default => [:compile] #, :test]

require 'rake/rdoctask'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "ptp4r #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end
