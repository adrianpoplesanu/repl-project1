Ad interpreter written in Typescript

Setup project:
https://dev.to/avalon-lab/setting-up-a-typescript-project-in-2021-4cfg

npm init
create tsconfig.json with content:
{
 //"extends": "@tsconfig/recommended/tsconfig.json",
 "compilerOptions": {
 "outDir": "./dist",
 },
 "include": ["src/**/*"],
 "exclude": ["node_modules", "dist"]
}

create src/demo.ts with content:
export function greeter(person: string): string {
 return "Hello, " + person + "!";
}

console.log(greeter("World"));

npm run build
node dist/demo.js

